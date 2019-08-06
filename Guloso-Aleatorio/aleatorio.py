# coding-UTF-8
from typing import List, Any
import random
import numpy as np

import math
Limiares=[50,2,30,20,10]


class aleatorio():

    def __init__(self, log=False):
        self.log = log

    def mudarFog(self, fog, Fogs):
        s = [x for x in Fogs if x not in fog]
        return (random.choice(s))

    def atualizarFogsEntrada(self, fogs,server):
        mudar=False
        parametro = np.array(fogs[server]).flat
        n=parametro[4]
        if n==0:
                n=1
        if (parametro[0] > Limiares[0] or (parametro[1]/n)<(Limiares[1]/n) or parametro[2] > Limiares[2] or parametro[3] > Limiares[3] or n>Limiares[4]):
            mudar = True
        if (mudar == True):
            return self.mudarFog(server, fogs)
        else:
            return server

    def Politica(self, matrizesdepreferencias,ip):
        ip=ip
        return  self.atualizarFogsEntrada(matrizesdepreferencias,ip)