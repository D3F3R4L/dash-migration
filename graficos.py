import numpy as np
import matplotlib.pyplot as plt
import matplotlib.lines as mlines
import os
import glob
import operator
import argparse

parser = argparse.ArgumentParser(description='Script to make graphs for dash migration simulation')
parser.add_argument('--segmentfile','-seg',default="segmentSizesBigBuck1A.txt", type=str,help='Name of segmentfile used.Default is segmentSizesBigBuck1A.txt')
parser.add_argument('--adaptAlgo','-Adpt',default="festive", type=str,help='Name of adaptation algorithm used.Default is festive, possible values are: festive, panda, tobasco')
parser.add_argument('--Clients','-c', type=int,help='Number of Clients in the simulation')
#parser.add_argument('--Politica','-p', type=int,help='Politica used in the simulation')
parser.add_argument('--runs','-r', type=int,default=1,help='Number of simulations to make the graphs. Default is 1')
args = parser.parse_args()
if args.adaptAlgo!="festive" and args.adaptAlgo!="panda" and args.adaptAlgo!="tobasco":
    parser.error("Wrong adaptation algorithm")
if args.runs <= 0 :
    parser.error("Invalid number of runs")

runs=args.runs
segmentfile=args.segmentfile
adaptAlgo=args.adaptAlgo
numberOfClients=args.Clients
#politica=args.Politica
MeansTotals=[]
MeansBW=[]
throughtputTotals=[]
timeTotals=[]
MMEsTotals=[]
qualityLevelTotals=[]
StallsTotals=[]
RebuffersTotals=[]


def main():
  print('Beginning')
  segfile='src/dash-migration/dash/{seg}'.format(seg=segmentfile)
  file = open(segfile,"r")
  collums= file.readline().split(" ")
  numSegments=len(collums)-1
  #os.chdir('..')
  folder='dash-log-files/{algo}/{num}'.format(algo=adaptAlgo,num=numberOfClients)
  folders=os.listdir(folder)
  os.chdir(folder)
  folders.sort(key=operator.itemgetter(0))
  aux=[]
  for i in folders:
    if i.isdigit():
      aux.append(i)
  folders=aux
  print(folders)
  for i in folders:
    #folder='dash-log-files/{algo}/{num}/{pol}'.format(algo=adaptAlgo,num=numberOfClients,pol=i)
    print('------Politica ',i,'------')
    os.chdir(i)
    #bufferUnderrunGraphs()
    #throughputGraphs(numSegments)
    qualityGraphs(numSegments)
    throughtputServer()
    StallsGraphs()
    RebufferGraphs()
    os.chdir('..')
  graphtotals(numSegments)
  print('Finished')
  #print(os.getcwd())
  #print(os.listdir())

###################
# Buffer Underrun 
###################
def bufferUnderrunGraphs():
  files= '*sim{simu}*bufferUnderrunLog*'.format(simu=simulation)
  bufferUnderrunFiles = glob.glob(files)
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
  plt.plot(x,S1Throughput,color='r')
  plt.plot(x,S2Throughput,color='g')
  plt.plot(x,S3Throughput,color='b')
  plt.plot(x,S4Throughput,color='y')
  plt.xlabel('Seconds')
  plt.ylabel('Mb/s')
  red_line = mlines.Line2D([], [], color='Red',markersize=15, label='Tier-2 EP-1')
  green_line = mlines.Line2D([], [], color='g',markersize=15, label='Tier-2 EP-2')
  blue_line = mlines.Line2D([], [], color='b',markersize=15, label='Tier-2 EP-3')
  yellow_line = mlines.Line2D([], [], color='y',markersize=15, label='Tier-1 Cloud')
  plt.legend(title='Enforcement Point',handles=[red_line,green_line,blue_line,yellow_line])
  plt.title("Server Throughput")
  save = 'ServerThroughput.png'
  plt.savefig(save,bbox_inches="tight",dpi=300)
  plt.close()

def throughtputServer():
  i=0
  Times=[]
  Throughputs=[]
  MMEs=[]
  TimesAux=[]
  ThroughputsAux=[]
  MMEsAux=[]
  print('Working in throughtputServer...')
  while(i<runs):
    simulation=i
    files= '*throughputServer*sim{simu}*'.format(simu=simulation)
    throughputFiles = glob.glob(files)
    throughputFiles.sort()
    print(throughputFiles)
    j=0
    while(j<len(throughputFiles)):
      name = throughputFiles[j]
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
      TimesAux.append(Time)
      ThroughputsAux.append(Mbps)
      MMEsAux.append(MME)
      j+=1
    i+=1
  for k in range(0,4):
    Times.append(min(TimesAux , key=len))
    timeTotals.append(Times[k])
    aux=ThroughputsAux[k::4]
    for h in range (0,len(aux)):
      aux[h]=aux[h][0:len(Times[k])]
    Throughputs.append(sum(np.array(aux))/runs)
    throughtputTotals.append(Throughputs[k])
    aux=MMEsAux[k::4]
    for h in range (0,len(aux)):
      aux[h]=aux[h][0:len(Times[k])]
    MMEs.append(sum(np.array(aux))/runs)
    MMEsTotals.append(MMEs[k])
  plt.plot(Times[0],Throughputs[0],color='r',ls='--',lw=3)
  plt.plot(Times[1],Throughputs[1],color='g',ls='-.',lw=3)
  plt.plot(Times[2],Throughputs[2],color='b',ls=':',lw=3)
  plt.plot(Times[3],Throughputs[3],color='y',ls='-',lw=1)
  plt.xlabel('Seconds')
  plt.ylabel('Mb/s')
  red_line = mlines.Line2D([], [], color='Red',ls='--',lw=3, label='Tier-2 EP-1')
  green_line = mlines.Line2D([], [], color='g',ls='-.',lw=3, label='Tier-2 EP-2')
  blue_line = mlines.Line2D([], [], color='b',ls=':',lw=3, label='Tier-2 EP-3')
  yellow_line = mlines.Line2D([], [], color='y',ls='-',lw=2, label='Tier-1 Cloud')
  plt.legend(title='Enforcement Point',handles=[red_line,green_line,blue_line,yellow_line])
  plt.grid(True)
  plt.title("Server Throughput")
  save = 'ServerThroughput.png'
  plt.savefig(save,bbox_inches="tight",dpi=300)
  plt.close()

  plt.plot(Times[0],MMEs[0],color='r')
  plt.plot(Times[1],MMEs[1],color='g')
  plt.plot(Times[2],MMEs[2],color='b')
  plt.plot(Times[3],MMEs[3],color='y')
  plt.xlabel('Seconds')
  plt.ylabel('Mb/s')
  red_line = mlines.Line2D([], [], color='Red',markersize=15, label='Tier-2 EP-1')
  green_line = mlines.Line2D([], [], color='g',markersize=15, label='Tier-2 EP-2')
  blue_line = mlines.Line2D([], [], color='b',markersize=15, label='Tier-2 EP-3')
  yellow_line = mlines.Line2D([], [], color='y',markersize=15, label='Tier-1 Cloud')
  plt.legend(title='Enforcement Point',handles=[red_line,green_line,blue_line,yellow_line])
  plt.title("Exponential Moving Average of Server Throughput")
  plt.grid(True)
  save = 'MMEServerThroughput.png'
  plt.savefig(save,bbox_inches="tight",dpi=300)
  plt.close()
  print('ThroughtputServer Done')

###################
# Playback Quality 
###################
def qualityGraphs(numSegments):
  k=0
  print('Working in QualityGraphs...')
  bestScore=0
  worstScore=10000000
  Means=[]
  MeansSV=[]
  bestClientQuality=[]
  bestClientServer=[]
  worstClientQuality=[]
  worstClientServer=[]
  S1QualityMean=[]
  S2QualityMean=[]
  S3QualityMean=[]
  S4QualityMean=[]
  S1ClientsMean=[]
  S2ClientsMean=[]
  S3ClientsMean=[]
  S4ClientsMean=[]
  while k<runs:
    simulation=k
    files= '*sim{simu}*playbackLog*'.format(simu=simulation)
    playbackFiles = glob.glob(files)
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
      ClientScore=sum(qualityLevel)

      if(ClientScore>bestScore):
        bestScore=ClientScore
        bestClientQuality=qualityLevel
        bestClientServer=Server
      elif(ClientScore<worstScore):
        worstScore=ClientScore
        worstClientQuality=qualityLevel
        worstClientServer=Server

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
    S1QualityMean.append(toBitrate(S1Quality))
    S2QualityMean.append(toBitrate(S2Quality))
    S3QualityMean.append(toBitrate(S3Quality))
    S4QualityMean.append(toBitrate(S4Quality))
    S1ClientsMean.append(S1Clients)
    S2ClientsMean.append(S2Clients)
    S3ClientsMean.append(S3Clients)
    S4ClientsMean.append(S4Clients)
    k+=1
  worstClientQuality=toBitrate(worstClientQuality)
  bestClientQuality=toBitrate(bestClientQuality)
  S1QualityMean=np.mean(S1QualityMean,axis=0)
  S2QualityMean=np.mean(S2QualityMean,axis=0)
  S3QualityMean=np.mean(S3QualityMean,axis=0)
  S4QualityMean=np.mean(S4QualityMean,axis=0)
  S1ClientsMean=np.mean(S1ClientsMean,axis=0)
  S2ClientsMean=np.mean(S2ClientsMean,axis=0)
  S3ClientsMean=np.mean(S3ClientsMean,axis=0)
  S4ClientsMean=np.mean(S4ClientsMean,axis=0)
  qualityLevelTotals.append(S1QualityMean)
  qualityLevelTotals.append(S2QualityMean)
  qualityLevelTotals.append(S3QualityMean)
  qualityLevelTotals.append(S4QualityMean)
  x=np.arange(0,numSegments)
  fig,ax =plt.subplots()
  p1,=plt.plot(x,S1QualityMean,color='r',markersize=15, label='Tier-2 EP-1')
  p2,=plt.plot(x,S2QualityMean,color='g',markersize=15, label='Tier-2 EP-2')
  p3,=plt.plot(x,S3QualityMean,color='b',markersize=15, label='Tier-2 EP-3')
  p4,=plt.plot(x,S4QualityMean,color='y',markersize=15, label='Tier-1 Cloud')
  plt.xlabel('Segments')
  plt.ylabel('Video bitrate(Kbps)')
  MeansSV.append(int(np.mean(S1QualityMean)))
  MeansSV.append(int(np.mean(S2QualityMean)))
  MeansSV.append(int(np.mean(S3QualityMean)))
  MeansSV.append(int(np.mean(S4QualityMean)))
  MeansTotals.append(MeansSV)
  red_line = mlines.Line2D([], [], color='r',markersize=15, label='{mean} Kbps'.format(mean=MeansSV[0]))
  green_line = mlines.Line2D([], [], color='g',markersize=15, label='{mean} Kbps'.format(mean=MeansSV[1]))
  blue_line = mlines.Line2D([], [], color='b',markersize=15, label='{mean} Kbps'.format(mean=MeansSV[2]))
  yellow_line = mlines.Line2D([], [], color='y',markersize=15, label='{mean} Kbps'.format(mean=MeansSV[3]))
  plt.yticks( [0,400,650,1000,1500,2250,3400,4700,6000], ('0','400', '650', '1000', '1500', '2250','3400','4700','6000') )
  l1=plt.legend(title='Enforcement Point',handles=[p1,p2,p3,p4],bbox_to_anchor=(1.04,1), loc="upper left",fancybox=True, shadow=True)
  plt.title("Video Bitrate")
  plt.legend(title='Bitrate Mean',handles=[red_line,green_line,blue_line,yellow_line],bbox_to_anchor=(1.04,0.5), loc="center left",fancybox=True, shadow=True)
  plt.grid(True)
  ax.add_artist(l1)
  save = 'qualityLevel.png'
  plt.savefig(save,bbox_inches="tight",dpi=300)
  plt.close()
  
  i=0
  fig,ax =plt.subplots()
  p1,=plt.plot([],[],color='r',markersize=15, label='Tier-2 EP-1')
  p2,=plt.plot([],[],color='g',markersize=15, label='Tier-2 EP-2')
  p3,=plt.plot([],[],color='b',markersize=15, label='Tier-2 EP-3')
  p4,=plt.plot([],[],color='y',markersize=15, label='Tier-1 Cloud')
  for x1, x2, y1,y2 in zip(x, x[1:], bestClientQuality, bestClientQuality[1:]):
    if str(bestClientServer[i])=="1.0.0.1":
      plt.plot([x1, x2], [y1, y2], 'r')
    elif str(bestClientServer[i])=="2.0.0.1":
      plt.plot([x1, x2], [y1, y2], 'g')
    elif(str(bestClientServer[i])=="3.0.0.1"):
      plt.plot([x1, x2], [y1, y2], 'b')
    else:
      plt.plot([x1, x2], [y1, y2], 'y')
    i+=1
  Means.append(int(np.mean(bestClientQuality)))
  red_line = mlines.Line2D([], [], color='black',markersize=15, label='{mean} Kbps'.format(mean=Means[0]))
  plt.yticks( [0,400,650,1000,1500,2250,3400,4700,6000], ('0','400', '650', '1000', '1500', '2250','3400','4700','6000') )
  l1=plt.legend(title='Enforcement Point',handles=[p1,p2,p3,p4],bbox_to_anchor=(1.04,1), loc="upper left",fancybox=True, shadow=True)
  ax.add_artist(l1)
  plt.grid(True)
  plt.xlabel('Segments')
  plt.ylabel('Video bitrate(Kbps)')
  plt.legend(title='Bitrate Mean',handles=[red_line],bbox_to_anchor=(1.04,0.5), loc="center left",fancybox=True, shadow=True)
  plt.title("Video Bitrate Best Client")
  save = 'qualityLevelBestClient.png'
  plt.savefig(save,bbox_inches="tight",dpi=300)
  plt.close()

  i=0
  fig,ax =plt.subplots()
  p1,=plt.plot([],[],color='r',markersize=15, label='Tier-2 EP-1')
  p2,=plt.plot([],[],color='g',markersize=15, label='Tier-2 EP-2')
  p3,=plt.plot([],[],color='b',markersize=15, label='Tier-2 EP-3')
  p4,=plt.plot([],[],color='y',markersize=15, label='Tier-1 Cloud')
  for x1, x2, y1,y2 in zip(x, x[1:], worstClientQuality, worstClientQuality[1:]):
    if str(worstClientServer[i])=="1.0.0.1":
      plt.plot([x1, x2], [y1, y2], 'r')
    elif str(worstClientServer[i])=="2.0.0.1":
      plt.plot([x1, x2], [y1, y2], 'g')
    elif(str(worstClientServer[i])=="3.0.0.1"):
      plt.plot([x1, x2], [y1, y2], 'b')
    else:
      plt.plot([x1, x2], [y1, y2], 'y')
    i+=1
  Means.append(int(np.mean(worstClientQuality)))
  if np.isnan(Means[1]):
    Means[1]=np.nan_to_num(Means[1])
  MeansBW.append(Means)
  red_line = mlines.Line2D([], [], color='black',markersize=15, label='{mean} Kbps'.format(mean=Means[1]))
  plt.yticks( [0,400,650,1000,1500,2250,3400,4700,6000], ('0','400', '650', '1000', '1500', '2250','3400','4700','6000') )
  l1=plt.legend(title='Enforcement Point',handles=[p1,p2,p3,p4],bbox_to_anchor=(1.04,1), loc="upper left",fancybox=True, shadow=True)
  ax.add_artist(l1)
  plt.grid(True)
  plt.xlabel('Segments')
  plt.ylabel('Video bitrate(Kbps)')
  plt.legend(title='Bitrate Mean',handles=[red_line],bbox_to_anchor=(1.04,0.5), loc="center left",fancybox=True, shadow=True)
  plt.title("Video Bitrate Worst Client")
  save = 'qualityLevelWorstClient.png'
  plt.savefig(save,bbox_inches="tight",dpi=300)
  plt.close()

  red_line = mlines.Line2D([], [], color='r',markersize=15, label='Tier-2 EP-1')
  green_line = mlines.Line2D([], [], color='g',markersize=15, label='Tier-2 EP-2')
  blue_line = mlines.Line2D([], [], color='b',markersize=15, label='Tier-2 EP-3')
  yellow_line = mlines.Line2D([], [], color='y',markersize=15,label='Tier-1 Cloud')
  width = 0.6
  fig,ax =plt.subplots()
  p1 = plt.bar(x, S1ClientsMean, width,color='r',align='edge')
  p2 = plt.bar(x, S2ClientsMean, width,bottom=S1ClientsMean,color='g',align='edge')
  p3 = plt.bar(x, S3ClientsMean, width,bottom=np.array(S1ClientsMean)+np.array(S2ClientsMean),color='b',align='edge')
  p4 = plt.bar(x, S4ClientsMean, width,bottom=np.array(S1ClientsMean)+np.array(S2ClientsMean)+np.array(S3ClientsMean),color='y',align='edge')
  xmajor_ticks = np.arange(0, (numSegments+1), 5)
  xminor_ticks = np.arange(0, (numSegments+1), 1)
  ymajor_ticks = np.arange(0, (S1ClientsMean[0]+S2ClientsMean[0]+S3ClientsMean[0]+S4ClientsMean[0]+1), 5)
  yminor_ticks = np.arange(0, (S1ClientsMean[0]+S2ClientsMean[0]+S3ClientsMean[0]+S4ClientsMean[0]+1), 1)
  ax.set_xticks(xmajor_ticks)
  ax.set_xticks(xminor_ticks, minor=True)
  ax.set_yticks(ymajor_ticks)
  ax.set_yticks(yminor_ticks, minor=True)
  ax.set_xlim(0, numSegments)
  ax.set_ylim(0, (S1ClientsMean[0]+S2ClientsMean[0]+S3ClientsMean[0]+S4ClientsMean[0]+1))
  ax.grid(which='both')
  plt.xlabel('Segments')
  plt.ylabel('Clients')
  plt.title('Clients per Server Mean')
  plt.legend(title='Enforcement Point',handles=[red_line,green_line,blue_line,yellow_line],bbox_to_anchor=(1.04,1), loc="upper left",fancybox=True, shadow=True)
  save = 'clientsPerServer.png'
  plt.savefig(save,bbox_inches="tight",dpi=300)
  plt.close()
  print('QualityGraphs Done')

def divisor(vet1,vet2):
  j=0
  resp=[]
  while j<len(vet1):
    if vet2[j]==0:
      resp.append(-1)
    else:
      resp.append(vet1[j]/vet2[j])
    j+=1
  return resp

def StallsGraphs():
  k=0
  collums=[]
  print('Working in StallLog...')
  while(k<runs):
    simulation=k
    files= '*sim{simu}*StallLog*'.format(simu=simulation)
    StallFile = glob.glob(files)
    j=0
    while(j<len(StallFile)):
      name = StallFile[j]
      with open(name) as f:
        data = f.readlines()
      fields = data[-1].split(";")
      collums.append(fields)
      j+=1
    k+=1
  barGraphs(collums,'Stalls')  
  print('StallLog Done')

def RebufferGraphs():
  k=0
  collums=[]
  print('Working in RebufferLog...')
  while (k<runs):
    simulation=k
    files= '*sim{simu}*RebufferLog*'.format(simu=simulation)
    RebufferFile = glob.glob(files)
    j=0
    while(j<len(RebufferFile)):
      name = RebufferFile[j]
      with open(name) as f:
        data = f.readlines()
      fields = data[-1].split(";")
      collums.append(fields)
      j+=1
    k+=1
  barGraphs(collums,'Rebuffer')
  print('RebufferLog Done')

def barGraphs(collums, graph):
  default=[[],[],[],[]]
  defaultMSE=[]
  defaultMean=[]
  MME=[[],[],[],[]]
  MMEMSE=[]
  MMEMean=[]
  l=1
  k=0
  while (k<runs):
    while(l<len(collums[k])-1):
      if (l%2!=0):
      	default[int((l/2)-0.5)].append(float(collums[k][l]))
      else:
        MME[int((l/2)-1)].append(float(collums[k][l]))
      l+=1
    l=1
    k+=1
  for i in range(0,4):
  	defaultMean.append(np.mean(default[i]))
  	defaultMSE.append(np.std(default[i]))
  	MMEMean.append(np.mean(MME[i]))
  	MMEMSE.append(np.std(MME[i]))
  ind = np.arange(len(default))
  width = 0.4  
  fig, ax = plt.subplots()
  rects1 = ax.bar(ind - width/2, defaultMean, width, yerr=defaultMSE,label='Media')
  rects2 = ax.bar(ind + width/2, MMEMean, width, yerr=MMEMSE,label='MME')
  if (graph=='Stalls'):
    StallsTotals.append(defaultMean)
    ax.set_ylabel('Quantity')
    ax.set_title('Number of Stalls per Server')
    ax.set_xticks(ind)
    ax.set_xticklabels(('Tier-2 EP-1', 'Tier-2 EP-2', 'Tier-2 EP-3', 'Tier-1 Cloud'))
    ax.legend()
    save = 'Stalls.png'
  else:
    RebuffersTotals.append(defaultMean)
    ax.set_ylabel('Seconds')
    ax.set_title('Rebuffers duration per Server')
    ax.set_xticks(ind)
    ax.set_xticklabels(('Tier-2 EP-1', 'Tier-2 EP-2', 'Tier-2 EP-3', 'Tier-1 Cloud'))
    ax.legend()
    save = 'Rebuffers.png'
  
  plt.savefig(save,bbox_inches="tight",dpi=300)
  plt.close()

def graphtotals(numSegments):
  print('Working in Comparation Graphs...')
  print('Throughputs Comparation...')
  for i in range (0,4):
    plt.plot(timeTotals[i],throughtputTotals[i],color='r',ls='--',lw=2)
    plt.plot(timeTotals[i+4],throughtputTotals[i+4],color='g',ls='-.',lw=2)
    plt.plot(timeTotals[i+8],throughtputTotals[i+8],color='b',ls=':',lw=2)
    plt.xlabel('Seconds')
    plt.ylabel('Throughput (Mb/s)')
    red_line = mlines.Line2D([], [], color='Red',markersize=15, label='F2V')
    green_line = mlines.Line2D([], [], color='g',markersize=15, label='Greedy')
    blue_line = mlines.Line2D([], [], color='b',markersize=15, label='Random')
    plt.legend(handles=[red_line,green_line,blue_line])
    name='Tier-2 EP-{pol}'.format(pol=(i+1))
    if i==3:
      name='Tier-1 Cloud'
    plt.grid(True)
    plt.title(name)
    plt.savefig(name,bbox_inches="tight",dpi=300)
    plt.close()
  print('Throughputs Comparation done')
  print('Throughputs MME Comparation...')
  for i in range (0,4):
    plt.plot(timeTotals[i],MMEsTotals[i],color='r',ls='--',lw=2)
    plt.plot(timeTotals[i+4],MMEsTotals[i+4],color='g',ls='-.',lw=2)
    plt.plot(timeTotals[i+8],MMEsTotals[i+8],color='b',ls=':',lw=2)
    plt.xlabel('Seconds')
    plt.ylabel('Throughput (Mb/s)')
    red_line = mlines.Line2D([], [], color='Red',markersize=15, label='F2V')
    green_line = mlines.Line2D([], [], color='g',markersize=15, label='Greedy')
    blue_line = mlines.Line2D([], [], color='b',markersize=15, label='Random')
    plt.legend(handles=[red_line,green_line,blue_line])
    name='EMA of Tier-2 EP-{pol}'.format(pol=(i+1))
    if i==3:
      name='EMA of Tier-1 Cloud'
    plt.grid(True)
    plt.title(name)
    plt.savefig(name,bbox_inches="tight",dpi=300)
    plt.close()
  print('Throughputs MME Comparation done')
  print('Bitrate Comparation...')
  for i in range (0,4):
    x=np.arange(0,numSegments)
    fig,ax =plt.subplots()
    p1,=plt.plot(x,qualityLevelTotals[i],color='r',ls='--',lw=3, label='F2V')
    p2,=plt.plot(x,qualityLevelTotals[i+4],color='g',ls='-.',lw=3, label='Greedy')
    p3,=plt.plot(x,qualityLevelTotals[i+8],color='b',ls=':',lw=3, label='Random')
    plt.xlabel('Segments')
    plt.ylabel('Video bitrate(Kbps)')
    red_line = mlines.Line2D([], [], color='r',markersize=15, label='Tier-2 EP-1')
    green_line = mlines.Line2D([], [], color='g',markersize=15, label='Tier-2 EP-2')
    blue_line = mlines.Line2D([], [], color='b',markersize=15, label='Tier-2 EP-3')
    yellow_line = mlines.Line2D([], [], color='y',markersize=15, label='Tier-1 Cloud')
    plt.yticks( [0,400,650,1000,1500,2250,3400,4700,6000], ('0','400', '650', '1000', '1500', '2250','3400','4700','6000') )
    l1=plt.legend(handles=[p1,p2,p3],bbox_to_anchor=(1.04,1), loc="upper left",fancybox=True, shadow=True)
    save = 'Tier-2 EP-{pol}'.format(pol=(i+1))
    if i==3:
      save = 'Tier-1 Cloud'
    plt.title(save)
    #plt.legend(title='Enforcement Point',handles=[red_line,green_line,blue_line,yellow_line],bbox_to_anchor=(1.04,0.5), loc="center left",fancybox=True, shadow=True)
    plt.grid(True)
    ax.add_artist(l1)
    plt.savefig(save,bbox_inches="tight",dpi=300)
    plt.close()
  print('Bitrate Comparation done')
  print('Stalls Comparation...')
  ind = np.arange(4)
  width = 0.2
  fig, ax = plt.subplots()
  rects1 = ax.bar(ind - width, StallsTotals[0], width,label='F2V')
  rects2 = ax.bar(ind, StallsTotals[1], width,label='Greedy')
  rects3 = ax.bar(ind + width, StallsTotals[2], width,label='Random')
  ax.set_ylabel('Number of Stall Events')
  ax.set_xticks(ind)
  ax.set_xticklabels(('Tier-2 EP-1', 'Tier-2 EP-2', 'Tier-2 EP-3', 'Tier-1 Cloud'))
  ax.legend()
  plt.grid(True,axis='y')
  save = 'StallsTotals.png'
  plt.savefig(save,bbox_inches="tight",dpi=300)
  plt.close()
  print('Stalls Comparation done')
  print('Rebuffers Comparation...')
  ind = np.arange(4)
  width = 0.2  
  fig, ax = plt.subplots()
  rects1 = ax.bar(ind - width, RebuffersTotals[0], width,label='F2V')
  rects2 = ax.bar(ind, RebuffersTotals[1], width,label='Greedy')
  rects3 = ax.bar(ind + width, RebuffersTotals[2], width,label='Random')
  ax.set_ylabel('Stall Duration (Seconds)')
  ax.set_xticks(ind)
  ax.set_xticklabels(('Tier-2 EP-1', 'Tier-2 EP-2', 'Tier-2 EP-3', 'Tier-1 Cloud'))
  ax.legend()
  plt.grid(True,axis='y')
  save = 'RebuffersTotals.png'
  plt.savefig(save,bbox_inches="tight",dpi=300)
  plt.close()
  print('Rebuffers Comparation done')
  print('Clients Comparation...')
  ind = np.arange(4)
  width = 0.2
  fig, ax = plt.subplots()
  rects1 = ax.bar(ind - width, MeansTotals[0], width,label='F2V')
  rects2 = ax.bar(ind, MeansTotals[1], width,label='Greedy')
  rects3 = ax.bar(ind + width, MeansTotals[2], width,label='Random')
  ax.set_ylabel('Bitrate Mean (Mbps)')
  ax.set_xticks(ind)
  ax.set_xticklabels(('Tier-2 EP-1', 'Tier-2 EP-2', 'Tier-2 EP-3', 'Tier-1 Cloud'))
  ax.legend()
  plt.grid(True,axis='y')
  save = 'BitrateTotals.png'
  plt.savefig(save,bbox_inches="tight",dpi=300)
  plt.close()
  ind = np.arange(2)
  width = 0.2
  fig, ax = plt.subplots()
  rects1 = ax.bar(ind - width, MeansBW[0], width,label='F2V')
  rects2 = ax.bar(ind, MeansBW[1], width,label='Greedy')
  rects3 = ax.bar(ind + width, MeansBW[2], width,label='Random')
  ax.set_ylabel('Bitrate Mean (Mbps)')
  ax.set_xticks(ind)
  ax.set_xticklabels(('Best Client', 'Worst Client'))
  ax.legend()
  plt.grid(True,axis='y')
  save = 'BestWorstTotals.png'
  plt.savefig(save,bbox_inches="tight",dpi=300)
  plt.close()
  print('Clients Comparation done.')
  print('Graphs Comparation done.')

  ind = np.arange(0.0,1.5,0.5)
  width = 0.2
  fig, ax = plt.subplots()
  aux=[np.sum(StallsTotals[0])/40,np.sum(StallsTotals[1])/40,np.sum(StallsTotals[2])/40]
  confInt=[1.96*(np.std(StallsTotals[0])/np.sqrt(33)/40),1.96*(np.std(StallsTotals[1])/np.sqrt(33)/40),1.96*(np.std(StallsTotals[2])/np.sqrt(33))/40]
  rects1 = ax.bar(ind, aux,width,yerr=confInt,color=(('tab:blue'),('tab:orange'),('tab:green')))
  #rects1 = ax.bar(ind, np.sum(StallsTotals[0]),width,yerr=(1.96*(np.std(StallsTotals[0])/np.sqrt(runs))),label='F2V')
  #rects2 = ax.bar(ind, np.sum(StallsTotals[1]), width,yerr=(1.96*(np.std(StallsTotals[1])/np.sqrt(runs))),label='Greedy')
  #rects3 = ax.bar(ind, np.sum(StallsTotals[2]),width,yerr=(1.96*(np.std(StallsTotals[2])/np.sqrt(runs))),label='Random')
  ax.set_ylabel('Number of Stall Events')
  ax.set_xticks(ind)
  ax.set_xticklabels(('F2V', 'Greedy', 'Random'))
  #ax.legend()
  plt.grid(True,axis='y')
  save = 'StallsTotal.png'
  plt.savefig(save,bbox_inches="tight",dpi=300)
  plt.close()
  ind = np.arange(3)
  width = 0.2  
  fig, ax = plt.subplots()
  aux=[np.sum(RebuffersTotals[0])/40,np.sum(RebuffersTotals[1])/40,np.sum(RebuffersTotals[2])/40]
  confInt=[1.96*(np.std(RebuffersTotals[0])/np.sqrt(33)/40),1.96*(np.std(RebuffersTotals[1])/np.sqrt(33)/40),1.96*(np.std(RebuffersTotals[2])/np.sqrt(33))/40]
  rects1 = ax.bar(ind, aux,width,yerr=confInt,color=(('tab:blue'),('tab:orange'),('tab:green')))
  #rects1 = ax.bar(ind - width, np.sum(RebuffersTotals[0]),width,yerr=(1.96*(np.std(RebuffersTotals[0])/np.sqrt(runs))),label='F2V')
  #rects2 = ax.bar(ind, np.sum(RebuffersTotals[1]), width,yerr=(1.96*(np.std(RebuffersTotals[1])/np.sqrt(runs))),label='Greedy')
  #rects3 = ax.bar(ind + width, np.sum(RebuffersTotals[2]),width,yerr=(1.96*(np.std(RebuffersTotals[2])/np.sqrt(runs))),label='Random')
  ax.set_ylabel('Stall Duration (Seconds)')
  ax.set_xticks(ind)
  ax.set_xticklabels(('F2V', 'Greedy', 'Random'))
  #ax.legend()
  plt.grid(True,axis='y')
  save = 'RebuffersTotal.png'
  plt.savefig(save,bbox_inches="tight",dpi=300)
  plt.close()

def toBitrate(vet):
  for i in range(0,len(vet)):
    if vet[i]==-1:
      vet[i]=0
    elif vet[i]<=1:
      vet[i]=250*vet[i]+400
    elif vet[i]<=2:
      vet[i]=350*(vet[i]-1)+650
    elif vet[i]<=3:
      vet[i]=500*(vet[i]-2)+1000
    elif vet[i]<=4:
      vet[i]=750*(vet[i]-3)+1500
    elif vet[i]<=5:
      vet[i]=1150*(vet[i]-4)+2250
    else:
      vet[i]=1300*(vet[i]-5)+3400
  return vet

if __name__=="__main__":
    main()
