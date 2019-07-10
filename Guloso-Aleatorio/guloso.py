from typing import List, Any

import numpy as np

import math

Limiares=[20, 40, 6, 10]
clients=[]

class guloso():

    def __init__(self, log=False):
        self.log = log

    def mudarFog(self, fog, Fogs):
        s = [x for x in Fogs if x not in fog]
        for i in s:
            mudar = False
            parametro = np.array(Fogs[i]).flat
            for n in range(0, 4):
                if parametro[n] < Limiares[n]:
                     mudar = True
                else:
                    mudar = False
            if (mudar == True):
                print("server ",i)
                return i
        return (Fogs[server])

    def atualizarFogsEntrada(self, fogs,server):
        mudar=False
        parametro =  np.array(fogs[server]).flat
        for n in range(0, 4):
            if (parametro[n] > Limiares[n] or server == "4.0.0.1"):
                mudar = True
        if (mudar == True):
            return self.mudarFog(server, fogs)
        else:
            return fogs[server]

    def Politica(self, matrizesdepreferencias,ip,clients):
        ip=ip
        clients=clients
        return self.atualizarFogsEntrada(matrizesdepreferencias,ip)
