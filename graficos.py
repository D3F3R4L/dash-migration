import numpy as np
import matplotlib.pyplot as plt
import matplotlib.lines as mlines
import os
import glob

def main():
  print('começou')
  os.chdir('..')
  segmentfile="segmentSizesBigBuck90.txt"
  segfile='src/dash-migration/dash/{seg}'.format(seg=segmentfile)
  file = open(segfile,"r")
  collums= file.readline().split(" ")
  numSegments=len(collums)-1
  adaptAlgo="festive"
  simulation=1
  numberOfClients=15
  folder='dash-log-files/{algo}/{num}'.format(algo=adaptAlgo,num=numberOfClients)
  os.chdir(folder)
  #bufferUnderrunGraphs()
  #throughputGraphs(numSegments)
  qualityGraphs(numSegments)
  throughtputServer()
  StallsGraphs()
  RebufferGraphs()
  print('terminou')
  #print(os.getcwd())
  #print(os.listdir())

###################
# Buffer Underrun 
###################
def bufferUnderrunGraphs():
  bufferUnderrunFiles = glob.glob('*bufferUnderrunLog*')
  print(bufferUnderrunFiles)
  S1timeTotal=[]
  S2timeTotal=[]
  S3timeTotal=[]
  j=0
  while(j<len(bufferUnderrunFiles)):
    print(j)
    name = bufferUnderrunFiles[j]
    file = open(name,"r")
    next(file)
    Buffer_Underrun_Total_Time=[]
    for line in file:
      fields = line.split(";")
      print(fields)

      if(j!=0):
        timeTotal.append(float(fields[3])+timeTotal[len(timeTotal)-1])
      else:
        timeTotal.append(float(fields[3]))
    if j==0 and len(timeTotal)==0 :
      timeTotal.append(0)
    file.close()
    j+=1
  
  plt.plot(totalUnderruns,timeTotal , label='linear')
  plt.xlabel('Buffer Underrun')
  plt.ylabel('Total Time')
  plt.title("Buffer Underrun Total Duration Time")
  save = 'BufferUnderrunTotalTime.png'
  plt.savefig(save)
  plt.close()

  S1Quality=np.zeros(numSegments)
  S2Quality=np.zeros(numSegments)
  S3Quality=np.zeros(numSegments)
  S1Clients=np.zeros(numSegments)
  S2Clients=np.zeros(numSegments)
  S3Clients=np.zeros(numSegments)
  j=0
  while(j<len(playbackFiles)):
    name = playbackFiles[j]
    file = open(name,"r")
    next(file)
    segment=[]
    qualityLevel=[]
    Server=[]
    for line in file:
      fields = line.split(";")
      segment.append(float(fields[0]))
      qualityLevel.append(float(fields[2]))
      Server.append(str(fields[3]))
    file.close()

    i=0
    for x1, x2, y1,y2 in zip(segment, segment[1:], qualityLevel, qualityLevel[1:]):
      if str(Server[i])=="1.0.0.1":
        S1Quality[i]=S1Quality[i]+y1
        S1Clients[i]=S1Clients[i]+1
      elif str(Server[i])=='2.0.0.1' :
        S2Quality[i]=S2Quality[i]+y1
        S2Clients[i]=S2Clients[i]+1
      else:
        S3Quality[i]=S3Quality[i]+y1
        S3Clients[i]=S3Clients[i]+1
      i+=1
    j+=1

'''
  Buffer_Underrun_Started_At.insert(0,0.0)
  Buffer_Underrun_Total_Time.insert(0,0.0)
  y1 = np.arange(len(Buffer_Underrun_Started_At))
  plt.step(Buffer_Underrun_Started_At, y1, where= 'post',label='')
  plt.plot(Buffer_Underrun_Started_At, y1, 'C0o', alpha=0.5)
  plt.xlabel('Segundos')
  plt.ylabel('Estouros')
  plt.title("Numero de estouros de buffer")
  save = 'sim{simu}_cl{iter}_NumbersOfBufferUnderrun.png'.format(simu=simulation, iter=j)
  plt.savefig(save)
  plt.close()
  plt.plot(Buffer_Underrun_Started_At,Buffer_Underrun_Total_Time , label='linear')
  plt.xlabel('Segundos')
  plt.ylabel('Tempo Total')
  plt.title("Buffer Underrun Total Duration Time")
  save = 'sim{simu}_cl{iter}_BufferUnderrunDurationTime.png'.format(simu=simulation, iter=j)
  plt.savefig(save)
  plt.close()
'''

################
# Throughput 
################
def throughputGraphs(numSegments):
  throughputFiles = glob.glob('*throughputLog*')
  print(throughputFiles)
  S1Throughput=np.zeros(numSegments)
  S2Throughput=np.zeros(numSegments)
  S3Throughput=np.zeros(numSegments)
  S4Throughput=np.zeros(numSegments)
  S1Clients=np.zeros(numSegments)
  S2Clients=np.zeros(numSegments)
  S3Clients=np.zeros(numSegments)
  S4Clients=np.zeros(numSegments)
  j=0
  while(j<len(throughputFiles)):
    name = throughputFiles[j]
    file = open(name,"r")
    next(file)
    Time=[]
    Mbps=[]
    Server=[]
    for line in file:
      fields = line.split(";")
      Time.append(float(fields[0]))
      Mbps.append(float(fields[1]))
      Server.append(str(fields[2]))
    file.close()
    i=0
    for x1, x2, y1,y2 in zip(Time, Time[1:], Mbps, Mbps[1:]):
      if str(Server[i])=="1.0.0.1":
        S1Throughput[i]=S1Throughput[i]+y1
        S1Clients[i]=S1Clients[i]+1
      elif str(Server[i])=='2.0.0.1' :
        S2Throughput[i]=S2Throughput[i]+y1
        S2Clients[i]=S2Clients[i]+1
      elif str(Server[i])=='3.0.0.1' :
        S3Throughput[i]=S3Throughput[i]+y1
        S3Clients[i]=S3Clients[i]+1
      else:
        S4Throughput[i]=S4Throughput[i]+y1
        S4Clients[i]=S4Clients[i]+1
      i+=1
    j+=1
  
  x = np.arange(0,2*(len(Time)),2)
  S1Throughput=divisor(S1Throughput,S1Clients)
  S2Throughput=divisor(S2Throughput,S2Clients)
  S3Throughput=divisor(S3Throughput,S3Clients)
  S4Throughput=divisor(S4Throughput,S4Clients)
  #np.insert(S1Throughput,0,0.0)
  #np.insert(S2Throughput,0,0.0)
  plt.plot(x,S1Throughput,color='r')
  plt.plot(x,S2Throughput,color='g')
  plt.plot(x,S3Throughput,color='b')
  plt.plot(x,S4Throughput,color='y')
  plt.xlabel('Seconds')
  plt.ylabel('Mb/s')
  red_line = mlines.Line2D([], [], color='Red',
                         markersize=15, label='Sv1')
  green_line = mlines.Line2D([], [], color='g',
                         markersize=15, label='Sv2')
  blue_line = mlines.Line2D([], [], color='b',
                        markersize=15, label='Sv3')
  yellow_line = mlines.Line2D([], [], color='y',
                        markersize=15, label='Cloud')
  plt.legend(title='Reinforcement Point',handles=[red_line,green_line,blue_line,yellow_line])
  plt.title("Server Throughput")
  save = 'ServerThroughput.png'
  plt.savefig(save)
  plt.close()

def throughtputServer():
  throughputFiles = glob.glob('*throughputServer*')
  print('Working in throughtputServer...')
  print(throughputFiles)
  Times=[]
  Throughputs=[]
  MMEs=[]
  j=0
  while(j<len(throughputFiles)):
    name = throughputFiles[len(throughputFiles)-1-j]
    file = open(name,"r")
    next(file)
    Time=[]
    Mbps=[]
    MME=[]
    for line in file:
      fields = line.split(";")
      Time.append(float(fields[0]))
      Mbps.append(float(fields[1]))
      MME.append(float(fields[2]))
    Times.append(Time)
    Throughputs.append(Mbps)
    MMEs.append(MME)
    j+=1
  k=0
  plt.plot(Times[0],Throughputs[0],color='r')
  plt.plot(Times[1],Throughputs[1],color='g')
  plt.plot(Times[2],Throughputs[2],color='b')
  plt.plot(Times[3],Throughputs[3],color='y')
  plt.xlabel('Seconds')
  plt.ylabel('Mb/s')
  red_line = mlines.Line2D([], [], color='Red',
                         markersize=15, label='Sv1')
  green_line = mlines.Line2D([], [], color='g',
                         markersize=15, label='Sv2')
  blue_line = mlines.Line2D([], [], color='b',
                        markersize=15, label='Sv3')
  yellow_line = mlines.Line2D([], [], color='y',
                        markersize=15, label='Cloud')
  plt.legend(title='Reinforcement Point',handles=[red_line,green_line,blue_line,yellow_line])
  plt.title("Server Throughput")
  save = 'ServerThroughput.png'
  plt.savefig(save)
  plt.close()

  plt.plot(Times[0],MMEs[0],color='r')
  plt.plot(Times[1],MMEs[1],color='g')
  plt.plot(Times[2],MMEs[2],color='b')
  plt.plot(Times[3],MMEs[3],color='y')
  plt.xlabel('Seconds')
  plt.ylabel('Mb/s')
  red_line = mlines.Line2D([], [], color='Red',
                         markersize=15, label='Sv1')
  green_line = mlines.Line2D([], [], color='g',
                         markersize=15, label='Sv2')
  blue_line = mlines.Line2D([], [], color='b',
                        markersize=15, label='Sv3')
  yellow_line = mlines.Line2D([], [], color='y',
                        markersize=15, label='Cloud')
  plt.legend(title='Reinforcement Point',handles=[red_line,green_line,blue_line,yellow_line])
  plt.title("Exponential Moving Average of Server Throughput")
  save = 'MMEServerThroughput.png'
  plt.savefig(save)
  plt.close()
  print('ThroughtputServer Done')

###################
# Playback Quality 
###################
def qualityGraphs(numSegments):
  playbackFiles = glob.glob('*playbackLog*')
  print('Working in QualityGraphs...')
  S1Quality=np.zeros(numSegments)
  S2Quality=np.zeros(numSegments)
  S3Quality=np.zeros(numSegments)
  S4Quality=np.zeros(numSegments)
  S1Clients=np.zeros(numSegments)
  S2Clients=np.zeros(numSegments)
  S3Clients=np.zeros(numSegments)
  S4Clients=np.zeros(numSegments)
  j=0
  while(j<len(playbackFiles)):
    name = playbackFiles[j]
    file = open(name,"r")
    next(file)
    segment=[]
    qualityLevel=[]
    Server=[]
    for line in file:
      fields = line.split(";")
      segment.append(float(fields[0]))
      qualityLevel.append(float(fields[2]))
      Server.append(str(fields[3]))
    file.close()

    i=0
    for x1, x2, y1,y2 in zip(segment, segment[1:], qualityLevel, qualityLevel[1:]):
      if str(Server[i])=="1.0.0.1":
        S1Quality[i]=S1Quality[i]+y1
        S1Clients[i]=S1Clients[i]+1
      elif str(Server[i])=='2.0.0.1' :
        S2Quality[i]=S2Quality[i]+y1
        S2Clients[i]=S2Clients[i]+1
      elif str(Server[i])=='3.0.0.1':
        S3Quality[i]=S3Quality[i]+y1
        S3Clients[i]=S3Clients[i]+1
      else:
        S4Quality[i]=S4Quality[i]+y1
        S4Clients[i]=S4Clients[i]+1
      i+=1
    j+=1
  S1Quality=divisor(S1Quality,S1Clients)
  S2Quality=divisor(S2Quality,S2Clients)
  S3Quality=divisor(S3Quality,S3Clients)
  S4Quality=divisor(S4Quality,S4Clients)
  x=np.arange(0,numSegments)
  plt.plot(x,S1Quality,color='r')
  plt.plot(x,S2Quality,color='g')
  plt.plot(x,S3Quality,color='b')
  plt.plot(x,S4Quality,color='y')
  plt.xlabel('Segments')
  plt.ylabel('Quality Level')
  red_line = mlines.Line2D([], [], color='Red',
                         markersize=15, label='Sv1')
  green_line = mlines.Line2D([], [], color='g',
                         markersize=15, label='Sv2')
  blue_line = mlines.Line2D([], [], color='b',
                        markersize=15, label='Sv3')
  yellow_line = mlines.Line2D([], [], color='y',
                        markersize=15, label='Cloud')
  plt.legend(title='Reinforcement Point',handles=[red_line,green_line,blue_line,yellow_line])
  plt.title("Quality Level")
  save = 'qualityLevel.png'
  plt.savefig(save)
  plt.close()
  print('QualityGraphs Done')

def divisor(vet1,vet2):
  j=0
  resp=[]
  while j<len(vet1):
    if vet2[j]==0:
      resp.append(0)
    else:
      resp.append(vet1[j]/vet2[j])
    j+=1
  return resp

def StallsGraphs():
  StallFile = glob.glob('*StallLog*')
  print('Working in StallLog...')
  j=0
  while(j<len(StallFile)):
    name = StallFile[j]
    file = open(name,"r")
    next(file)
    collums=[]
    for line in file:
      fields = line.split(";")
      collums.append(fields)
    collums=list(zip(*collums))
    barGraphs(collums,'Stalls')  
    j+=1

  print('StallLog Done')

def RebufferGraphs():
  RebufferFile = glob.glob('*RebufferLog*')
  print('Working in RebufferLog...')
  j=0
  while(j<len(RebufferFile)):
    name = RebufferFile[j]
    file = open(name,"r")
    next(file)
    collums=[]
    for line in file:
      fields = line.split(";")
      collums.append(fields)
    collums=list(zip(*collums))
    barGraphs(collums,'Rebuffer')  
    j+=1
  print('RebufferLog Done')

def barGraphs(collums, graph):
  default=[]
  defaultMSE=[]
  MME=[]
  MMEMSE=[]
  l=1
  while(l<len(collums)-1):
    if (l%2==0):
      mean=np.mean(list(map(float,collums[l])))
      mse=np.std(list(map(float,collums[l])))
      default.append(mean)
      defaultMSE.append(mse)
    else:
      mean=np.mean(list(map(float,collums[l])))
      mse=np.std(list(map(float,collums[l])))
      MME.append(mean)
      MMEMSE.append(mse)
    l+=1

  ind = np.arange(len(default))
  width = 0.4  
  fig, ax = plt.subplots()
  rects1 = ax.bar(ind - width/2, default, width, yerr=defaultMSE,
                  label='Media')
  rects2 = ax.bar(ind + width/2, MME, width, yerr=MMEMSE,
                  label='MME')
  if (graph=='Stalls'):
    ax.set_ylabel('Quantity')
    ax.set_title('Number of Stalls per Server')
    ax.set_xticks(ind)
    ax.set_xticklabels(('SV1', 'SV2', 'SV3', 'Cloud'))
    ax.legend()
    save = 'Stalls.png'
  else:
    ax.set_ylabel('Seconds')
    ax.set_title('Rebuffers duration per Server')
    ax.set_xticks(ind)
    ax.set_xticklabels(('SV1', 'SV2', 'SV3', 'Cloud'))
    ax.legend()
    save = 'Rebuffers.png'
  
  plt.savefig(save)
  plt.close()
if __name__=="__main__":
    main()
