"""
@author  Daniel.Krajzewicz@dlr.de
@date    2014-09-01
@version $Id: __init__.py 3774 2014-08-06 10:23:19Z erdm_ja $

Copyright (C) 2014 DLR/TS, Germany
All rights reserved
"""


from . import *
import os
import sumolib.net.generator.demand as demandGenerator
from sumolib.net.generator.network import *



      
flowsRiLSA1 = [
   [ "nmp", [
       [ "ms", 359, 9 ],  
       [ "me", 59, 9 ],  
       [ "mw", 64, 12 ]  
   ] ],

   [ "wmp", [
       [ "me", 508, 10 ],  
       [ "mn", 80, 14 ],  
       [ "ms", 130, 2 ]  
   ] ],

   [ "emp", [
       [ "mw", 571, 10 ],  
       [ "mn", 57, 9 ],  
       [ "ms", 47, 3 ]  
   ] ],

   [ "smp", [
       [ "mn", 354, 2 ],  
       [ "me", 49, 2 ],  
       [ "mw", 92, 2 ]  
   ] ]
 
]

 
 


class Scenario_RiLSA1OutTLS(Scenario):
  NAME = "RiLSA1OutTLS"
  THIS_DIR = os.path.join(os.path.abspath(os.path.dirname(__file__)), NAME)
  TLS_FILE = os.path.join(THIS_DIR, "tls.add.xml")
  NET_FILE = os.path.join(THIS_DIR, "rilsa1.net.xml") 

  def __init__(self, params, withDefaultDemand=True):
    Scenario.__init__(self, self.NAME)
    self.params = params
    self.netName = self.fullPath(self.NET_FILE)
    self.demandName = self.fullPath("routes.rou.xml")
    # network
    if fileNeedsRebuild(self.netName, "netconvert"):
      netconvert = sumolib.checkBinary("netconvert")
      retCode = subprocess.call([netconvert, "-c", os.path.join(self.THIS_DIR, "build.netc.cfg")])
    # build the demand model (streams)
    if withDefaultDemand:
      self.demand = demandGenerator.Demand()
      for f in flowsRiLSA1:
        for rel in f[1]:
          prob = rel[2]/100.
          iprob = 1. - prob
          
          pkwEprob = iprob * self.params["equippment-rate"]
          pkwNprob = iprob - pkwEprob
          lkwEprob = prob * self.params["equippment-rate"]
          lkwNprob = prob - lkwEprob
          print "%s %s %s %s %s" % (pkwEprob, pkwNprob, lkwEprob, lkwNprob, (pkwEprob+pkwNprob+lkwEprob+lkwNprob))
          
          self.demand.addStream(demandGenerator.Stream(f[0]+"__"+rel[0], 0, 3600, rel[1], f[0], rel[0], 
            { pkwEprob:"passenger", pkwNprob:"COLOMBO_undetectable_passenger", lkwEprob:"hdv", lkwNprob:"COLOMBO_undetectable_hdv" }))
      if fileNeedsRebuild(self.demandName, "duarouter"):
        self.demand.build(0, 3600, self.netName, self.demandName)
