from aleatorio import aleatorio
from guloso import guloso
import glob
import os
import sys
import operator
import csv

folder=sys.argv[1]
Type=sys.argv[2]
clients=[sys.argv[3],sys.argv[4],sys.argv[5],sys.argv[6]]
ip=sys.argv[7]
Servers={}
ServersIP=["1.0.0.1","2.0.0.1","3.0.0.1","4.0.0.1"]
gul = guloso(log=True)
al = aleatorio(log=True)

def main():
  os.chdir(folder)
  concatenarServers()
  if (Type=="guloso"):
    selecionado=gul.Politica(Servers,ip,clients)
  else:
    selecionado=al.Politica(Servers,ip,clients)
  print(selecionado)

def concatenarServers():
  StallValues=[0,0,0,0]
  RebufferValues=[0,0,0,0]
  ThroughputValues=[0,0,0,0]
  RebufferFile = glob.glob('*RebufferLog*')
  name = RebufferFile[0]
  file = open(name,"r")
  next(file)
  for line in file:
    fields = line.split(";")
    RebufferValues[0]=float(fields[1])
    RebufferValues[1]=float(fields[3])
    RebufferValues[2]=float(fields[5])
    RebufferValues[3]=float(fields[7])

    StallFile = glob.glob('*StallLog*')
    name = StallFile[0]
    file = open(name,"r")
    next(file)
    for line in file:
      fields = line.split(";")
      StallValues[0]=int(float(fields[1]))
      StallValues[1]=int(float(fields[3]))
      StallValues[2]=int(float(fields[5]))
      StallValues[3]=int(float(fields[7]))

    throughputFiles = glob.glob('*throughputServer*')
    throughputFiles.sort()
    j=0
    while(j<len(throughputFiles)):
      name = throughputFiles[j]
      file = open(name,"r")
      next(file)
      for line in file:
        fields = line.split(";")
        ThroughputValues[j]=float(fields[1])
      j+=1

    for i in range(0,4):
      ServerIP = ServersIP[i]
      Servers[ServerIP] = [RebufferValues[i],StallValues[i],ThroughputValues[i],int(float(clients[i]))]
    return Servers

if __name__=="__main__":
    main()
