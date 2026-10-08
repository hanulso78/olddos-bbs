# ZIL 소스의 TELL 에서 "문장 틀" 을 뽑는다: 한 줄 안의 글 조각 + D (물건 이름) + N (숫자)
#   예) <TELL "There is nothing behind the " D ,PRSO "." CR>  ->  "There is nothing behind the %o."
# 실행기의 줄 키와 같게: 글 조각은 게임 파일의 실제 문장(zork_strings)으로 맞추고, 줄바꿈(| 와 CR)에서 나눈다.
import re, glob, sys
SP = '/mnt/c/Users/MIN/AppData/Local/Temp/claude/D--Works-olddos-bbs/926433fe-f75d-48df-9a5d-d4004bf545bf/scratchpad'
src = ''
for f in sorted(glob.glob(SP + '/zork1/*.zil')):
	src += open(f, encoding='latin-1').read() + '\n'

def unesc(s):
	o = []; i = 0
	while i < len(s):
		if s[i] == '\\' and i + 1 < len(s):
			i += 1; o.append({'n': '\n'}.get(s[i], s[i]))
		else: o.append(s[i])
		i += 1
	return ''.join(o)
real = [unesc(l[3:].rstrip('\n')) for l in open(SP + '/zork_strings.txt', encoding='utf-8') if l.startswith('@@ ')]
norm = lambda t: re.sub(r'\s+', ' ', t)
by_norm = {}
for r in real: by_norm.setdefault(norm(r), r)

def zil_string(lit):
	# ZIL 문자열: \" 는 ", | 는 줄바꿈, 소스의 줄바꿈은 빈칸
	t = lit.replace('\\"', '"').replace('\r', '')
	t = re.sub(r'\n\s*', ' ', t) if False else t.replace('\n', ' ')
	t = t.replace('|', '\n')
	return t

def real_of(t):
	return by_norm.get(norm(t))

# 토큰 나누기 (TELL 안)
def tokens(body):
	i = 0; out = []
	while i < len(body):
		c = body[i]
		if c.isspace(): i += 1; continue
		if c == '"':
			j = i + 1
			while j < len(body) and not (body[j] == '"' and body[j - 1] != '\\'): j += 1
			out.append(('S', body[i + 1:j])); i = j + 1; continue
		if c in '<(':
			depth = 0; j = i
			while j < len(body):
				if body[j] == '"':
					j += 1
					while j < len(body) and not (body[j] == '"' and body[j - 1] != '\\'): j += 1
				elif body[j] in '<(': depth += 1
				elif body[j] in '>)':
					depth -= 1
					if depth == 0: break
				j += 1
			out.append(('F', body[i:j + 1])); i = j + 1; continue
		j = i
		while j < len(body) and not body[j].isspace() and body[j] not in '<>()"': j += 1
		out.append(('A', body[i:j])); i = max(j, i + 1)
	return out

# <TELL ...> 찾기 (균형 맞춰서)
tells = []
for m in re.finditer(r'<TELL\b', src):
	i = m.end(); depth = 1; j = i
	while j < len(src) and depth:
		if src[j] == '"':
			j += 1
			while j < len(src) and not (src[j] == '"' and src[j - 1] != '\\'): j += 1
		elif src[j] == '<': depth += 1
		elif src[j] == '>': depth -= 1
		j += 1
	tells.append(src[i:j - 1])

templates = {}
for body in tells:
	toks = tokens(body)
	line = []          # ('t', text) / ('o',) / ('n',)
	ok = True
	k = 0
	def flush():
		global line
		if any(p[0] in 'on' for p in line) and any(p[0] == 't' for p in line):
			key = ''.join(p[1] if p[0] == 't' else ('%o' if p[0] == 'o' else '%n') for p in line)
			if key.strip(): templates[key] = templates.get(key, 0) + 1
		line = []
	while k < len(toks):
		t, v = toks[k]
		if t == 'S':
			r = real_of(zil_string(v))
			if r is None: line = []; k += 1; continue     # 게임 파일에 없는 문장 (다른 판)
			parts = r.split('\n')
			for pi, part in enumerate(parts):
				if part: line.append(('t', part))
				if pi < len(parts) - 1: flush()
		elif t == 'A' and v in ('D', 'A'):
			line.append(('o',)); k += 1
		elif t == 'A' and v == 'N':
			line.append(('n',)); k += 1
		elif t == 'A' and v in ('CR', 'CRLF'):
			flush()
		else:
			# 문자열을 담은 전역 변수 등: 알 수 없으니 이 줄은 버린다
			line = []
		k += 1
	flush()

# 이미 번역표에 있는 틀은 빼고
ko = set()
for l in open('/mnt/d/Works/olddos-bbs/src/zork1/ko.txt', encoding='utf-8'):
	if l.startswith('@@ '): ko.add(unesc(l[3:].rstrip('\n')))
keys = sorted(k for k in templates if k not in ko)
def esc(s): return s.replace('\\', '\\\\').replace('\n', '\\n')
with open(SP + '/zk_tmpl_all.txt', 'w', encoding='utf-8') as f:
	for k in keys: f.write('@@ ' + esc(k) + '\n== \n')
print(len(tells), 'TELLs,', len(templates), 'templates,', len(keys), 'new')
for k in keys[:15]: print(repr(k))
