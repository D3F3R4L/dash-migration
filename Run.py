import os
import sys
import multiprocessing as mp
import numpy as np
import argparse

NumCores=os.cpu_count()
parser = argparse.ArgumentParser(description='Script to run multiple simulations with NS3 scripts, and balence the use of cores on CPU to always left some free cores')
parser.add_argument('script', type=str,help='The script local to run the script. Ex:scratch/myscript')
parser.add_argument('--arguments','-args', type=str,help='The arguments used in the script if needed. Ex: NumClientes=30  Type=2')
parser.add_argument('--cores','-c', type=float,default=0.5,help='Percentage of cores that the simulations can use. Default is 0.5')
parser.add_argument('--MaxUse','-m', action='store_true',help='Active max performance and disable balanced use of cores on CPU')
parser.add_argument('--runs','-r', type=int,default=1,help='Number of simulations that you like to run. Default is 1')
parser.add_argument('--id','-i', type=str,help='If simulation need a id the number of runs defined you be used, parse the command used in the simulations. Ex: SimulationId')
parser.add_argument('--seed','-s', type=str,help='If simulation need a seed the number of runs defined you be used, parse the command used in the simulations. Ex: SeedNumber')
args = parser.parse_args()
if args.cores > 1 or args.cores<0:
    parser.error("Number of cores exceed the existent Cores on your CPU")
if args.runs <= 0 :
    parser.error("Invalid number of runs")

Cores={}
NumSims=args.runs
MaxLoad=args.cores
MaxCores=int(NumCores*MaxLoad)
script=args.script
if args.arguments!=None:
	script=script+" --"+args.arguments
if args.id!=None:
	script=script+" --"+args.id
if args.seed!=None:
		script=script+' --'+args.seed
script = './waf --run="'+script
print(script)

def main():
	for i in range (0,MaxCores):
		var = 'core{num}'.format(num=i)
		Cores[var]=i
		print(Cores)
	j=0
	while j<NumSims:
		freeCores=int(np.ceil(NumCores-os.getloadavg()[0]))
		useCores=int(freeCores*MaxLoad)
		if (useCores+j)>NumSims:
			useCores=NumSims-j
		for i in range(0,useCores):
			Cores[list(Cores)[i]]=mp.Process(target=callFunction, args=(j,script,))
			j+=1
		for i in range(0,useCores):
			Cores[list(Cores)[i]].start()
		for i in range(0,useCores):
			Cores[list(Cores)[i]].join()

def callFunction(num, script):
	if args.id!=None:
		script=script+'={id}'.format(id=num)
	elif args.seed!=None:
		script=script+'={id}'.format(id=num)
	
	script=script+'"'
	print(script)
	print('Simulation{id}"'.format(id=num))
	os.system(script)

if __name__=="__main__":
    main()