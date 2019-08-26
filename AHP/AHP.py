from AnalyticHierarchyProcess import AHP
import glob
import os
import sys
import operator
import csv

folder=sys.argv[1]
simu=sys.argv[2]
delays=[sys.argv[3],sys.argv[4],sys.argv[5],sys.argv[6]]
ip=sys.argv[7]
Servers={}
ServersIP=["1.0.0.1","2.0.0.1","3.0.0.1","4.0.0.1"]
ahp = AHP (log=True)

def main():
  os.chdir(folder)
  concatenarServers()
  ServersScores=ahp.Politica(Servers,ip)
  ServersScores=list(ServersScores.items())
  ServersScores.sort(key=operator.itemgetter(1))
  #print(ServersScores)
  print(ServersScores[2][0],ServersScores[1][0],ServersScores[0][0])
  ServersScores.sort(key=operator.itemgetter(0))
  line=[str(ServersScores[0][1]),str(ServersScores[1][1]),str(ServersScores[2][1])]
  csv.register_dialect('myDialect',delimiter = ';',quoting=csv.QUOTE_NONE,skipinitialspace=True)
  file='sim{simu}_ServerScores.csv'.format(simu=simu)
  with open(file, 'a') as writeFile:
    writer = csv.writer(writeFile, dialect='myDialect')
    writer.writerow(line)
  writeFile.close()

def concatenarServers():
  StallValues=[0,0,0,0]
  RebufferValues=[0,0,0,0]
  ThroughputValues=[0,0,0,0]
  files= '*sim{simu}_RebufferLog*'.format(simu=simu)
  RebufferFile = glob.glob(files)
  name = RebufferFile[0]
  file = open(name,"r")
  next(file)
  for line in file:
    fields = line.split(";")
    RebufferValues[0]=float(fields[1])
    RebufferValues[1]=float(fields[3])
    RebufferValues[2]=float(fields[5])
    RebufferValues[3]=float(fields[7])

  files= '*sim{simu}_StallLog*'.format(simu=simu)
  StallFile = glob.glob(files)
  name = StallFile[0]
  file = open(name,"r")
  next(file)
  for line in file:
    fields = line.split(";")
    StallValues[0]=int(float(fields[1]))
    StallValues[1]=int(float(fields[3]))
    StallValues[2]=int(float(fields[5]))
    StallValues[3]=int(float(fields[7]))

  files= '*throughputServer_sim{simu}*'.format(simu=simu)
  throughputFiles = glob.glob(files)
  throughputFiles.sort()
  j=0
  while(j<len(throughputFiles)):
    name = throughputFiles[j]
    file = open(name,"r")
    next(file)
    for line in file:
      fields = line.split(";")
      ThroughputValues[j]=float(fields[2])
    j+=1

  for i in range(0,len(delays)):
    aux=list(delays[i])
    #print(delays)
    aux=aux[:-10]
    aux.pop(0)
    delays[i] = ''.join(aux)
    delays[i] = int(delays[i])

  for i in range(0,4):
    ServerIP = ServersIP[i]
    #if i==0 or i==1 or i==2:
    #StallValues[i]=i
    #if i==3:
    #  print('Delays: ',delays)
    #  print('ThroughputValues: ',ThroughputValues)
    #  print('Stalls: ',StallValues)
    #  print('Rebuffers: ',RebufferValues)
    #Servers[ServerIP] = [delays[i],ThroughputValues[i],StallValues[i],RebufferValues[i]]
    Servers[ServerIP] = [StallValues[i],RebufferValues[i],ThroughputValues[i],delays[i]]
  return Servers

if __name__=="__main__":
    main()