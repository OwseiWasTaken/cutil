#TODO: clean up the code
#TODO: make segments // [A-Z] usefull
#TODO: add colors


code = []
read = [[]]
defs = []
with open("cutil.h") as file:
	code+=file.read().splitlines()
with open("stuff.c") as file:
	code+=file.read().splitlines()
#with open("keys.c") as file:
#	code+=file.read().splitlines()

class cdef:
	def __init__(this, Type, Name, Args, Cont):
		this.Type = Type
		this.Name = Name
		this.Cont = Cont
		this.Args = Args
	def __str__(this):
		if this.Type == "typedef":
			return this.Name+' {\n\t'+'\n\t'.join(this.Cont).strip()+'\n}'
		elif this.Type == "define":
			return f"define {this.Name} = {this.Cont}"
		elif this.Type == "typeassing":
			return f"{this.Name} => {' '.join(this.Cont)}"
		elif this.Type == "global":
			return f"global {this.Name} {this.Cont}"
		return f"{this.Name}({', '.join(this.Args)}) -> {this.Type}"
	def __repr__(this):
		return f"{this.Name} -> {this.Type}\n" + '\t' + '\n\t'.join(this.Cont)
	def rec(this):
		# rebuild C code
		if this.Type != "typedef":
			return f"{this.Type} {this.Name}({', '.join(this.Args)});"
		else:
			return (
				"typedef struct {\n"
			)+'\t'+(
				';\n\t'.join(defs[-1].Cont)
			)+f";\n}} {this.Name};"
			return "re-c typedef not implemented"

def CleanLine(line):
	for j in range(len(line)):
		if line[j] == '/' and line[j-1] == '/' and j:
			line = line[:j-1]
			break
	if not line:
		return
	return line

for i in range(len(code)):
	line = CleanLine(code[i])
	if not line:continue
	line = line.removesuffix("{").rstrip()
	if not line:continue
	if line[0] == '\t':
		#print(line.replace('\t', "  "))
		read[-1].append(line.strip())
	elif line[0] == '}':
		#print() # end of function
		if len(line)-1: # in case of struct typedef
			read[-1].append(line[1:])
		read.append([])
	elif line[-1] == ';':
		if read[-1]:
			read.append([])
		read[-1].append(line)
		read.append([])
	elif line[0] == '#':
		if read[-1]:
			read.append([])
		read[-1].append(line)
		read.append([])
	else:
		#print(line)
		read[-1].append(line)

read = read[:-1] # remove last EOF

for func in read:
	vrs = {}
	name = func[0]
	lname = name.split(' ', 2)
	Cont = list(map(lambda x: x.replace(';', ''), func[1:]))
	Args = []
	if lname[0][0] == '#' and lname[0] != "#define":continue

	if lname[0] == "typedef":
		if name[-1] == ';':
			Type = "typeassing"
			lname = name.split(' ')
			Cont = lname[1:-1]
			Name = lname[-1][:-1]
		else:
			Type = "typedef"
			Name = func[-1][:-1].strip() # remove ' ' ';'
			Cont.pop(-1)
	elif lname[0] == "#define":
		Type, Name, Cont = lname
		Type = Type[1:]
	elif name[-1] == ';': # global def
		Type = "global"
		Name, Cont = lname
	else:
		Type, Name, Args = lname
		for r in "();": # remove char from string
			Args = Args.replace(r, "")
		Args = Args.split(',')
	defs.append(cdef(Type, Name, Args, Cont))

for d in defs:
	print(str(d))
