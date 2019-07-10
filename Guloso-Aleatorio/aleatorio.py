# coding-UTF-8
from typing import List, Any
import random
import numpy as np

import math
Limiares=[30, 20, 1, 10]


class aleatorio():

    def __init__(self, log=False):
        self.log = log

    def mudarFog(self, fog, Fogs):
        s = [x for x in Fogs if x not in fog]
        return (random.choice(s))

    def atualizarFogsEntrada(self, fogs,server):
        mudar=False
        parametro = np.array(fogs[server]).flat
        n=parametro[3]
        if n==0:
                n=1
        if (parametro[0] > Limiares[0] or parametro[1] > Limiares[1] or (parametro[2]/n)<(Limiares[2]/n) or n>Limiares[3]):
            mudar = True
        if (mudar == True):
            return self.mudarFog(server, fogs)
        else:
            return server

    def Politica(self, matrizesdepreferencias,ip):
        ip=ip
        return  self.atualizarFogsEntrada(matrizesdepreferencias,ip)