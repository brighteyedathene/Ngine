filename = "./Models/buddha/buddha-D.dae.unfixed"

import pdb; pdb.set_trace()

f = open(filename,"r")

lines = f.readlines()

f.close()

f = open(filename+".fixed","w")

for line in lines:
	if line.find('<param name="A" type="float"/>') == -1:
		f.write(line)

f.close()