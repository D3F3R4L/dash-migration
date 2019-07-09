# coding-UTF-8
from typing import List, Any
import random
import numpy as np

import math
Limiares=[2, 5, 10, 20]


class aleatorio():

    def __init__(self, log=False):
        self.log = log

    def mudarFog(self, fog, Fogs):
        s = [x for x in Fogs if x not in fog]
        return (random.choice(s))

    def atualizarFogsEntrada(self, fogs):
        mudar=False
        parametro = np.array(fogs["4.0.0.1"]).flat
        for n in range(0, 4):
            if (parametro[n] > Limiares[n]):
                mudar = True
        if mudar == True:
            return self.mudarFog("4.0.0.1", fogs)
        else:
            return fogs["4.0.0.1"]

    def Politica(self, matrizesdepreferencias,ip):
        server=ip
        return  self.atualizarFogsEntrada(matrizesdepreferencias)