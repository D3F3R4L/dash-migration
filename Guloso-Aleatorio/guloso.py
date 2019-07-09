from typing import List, Any

import numpy as np

import math

Limiares=[20, 40, 60, 3]

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
               return i
        return (Fogs[server])

    def atualizarFogsEntrada(self, fogs):
        mudar=False
        parametro =  np.array(fogs[server]).flat
        for n in range(0, 4):
            if (parametro[n] > Limiares[n]):
                mudar = True
        if (mudar == True):
            return self.mudarFog(server, fogs)
        else:
            return fogs[server]

    def Politica(self, matrizesdepreferencias,ip):
        server=ip
        return self.atualizarFogsEntrada(matrizesdepreferencias)
