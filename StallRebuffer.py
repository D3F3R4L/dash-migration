import glob
import numpy as np
import sys
import os

folder=sys.argv[1]
simulation=sys.argv[2]
#pol=sys.argv[3]

def main():
  os.chdir(folder)
  #folder='dash-log-files/{algo}/{num}/{pol}'.format(algo=adaptAlgo,num=numberOfClients,pol=pol)
  resp=bufferUnderrunGraphs()
  print(resp[0],resp[1],resp[2],resp[3],resp[4],resp[5],resp[6],resp[7])

###################
# Buffer Underrun 
###################
def bufferUnderrunGraphs():
  files= '*sim{simu}*bufferUnderrunLog*'.format(simu=simulation)
  bufferUnderrunFiles = glob.glob(files)
  #print(bufferUnderrunFiles)
  S1timeTotal=0
  S2timeTotal=0
  S3timeTotal=0
  CloudtimeTotal=0
  S1Nstalls=0
  S2Nstalls=0
  S3Nstalls=0
  CloudNstalls=0
  j=0
  while(j<len(bufferUnderrunFiles)):
    name = bufferUnderrunFiles[j]
    with open(name) as f:
      data = f.readlines()
    for i in range(1,len(data)):
      fields = data[i].split(";")
      #print(fields)
      if len(fields)>=5:
        if str(fields[0])=="1.0.0.1":
          S1Nstalls+=1
          S1timeTotal+=float(fields[3])
        elif str(fields[0])=='2.0.0.1' :
          S2Nstalls+=1
          S2timeTotal+=float(fields[3])
        elif str(fields[0])=='3.0.0.1' :
          S3Nstalls+=1
          S3timeTotal+=float(fields[3])
        else:
          CloudNstalls+=1
          CloudtimeTotal+=float(fields[3])
    f.close()
    j+=1
  return S1Nstalls,S1timeTotal,S2Nstalls,S2timeTotal,S3Nstalls,S3timeTotal,CloudNstalls,CloudtimeTotal

if __name__=="__main__":
    main()