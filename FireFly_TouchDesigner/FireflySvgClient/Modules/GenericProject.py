"""
Extension classes enhance TouchDesigner component networks with python
functionality. An extension can be accessed via ext.ExtensionClassName from
any operator within the extended component. If the extension is "promoted", all
its attributes with capitalized names can be accessed directly through the
extended component, e.g. op('yourComp').ExtensionMethod()
"""

import os
import socket

mainRolesDict = {
			"Ipmachine1" : "Machine 1",
			"Ipmachine2" : "Machine 2",
			"Ipmachine3" : "Machine 3"
			}
			

class GenericProject:
	"""
	GenericProject is the super class for all projects, it is inheritted by each
	project
	"""
	def __init__(self, ownerComp):
		# The component to which this extension is attached
		self.ownerComp = ownerComp
		self.assetsBase = self.ownerComp.op('assets')
		self.configBase = self.ownerComp.op('config')

	@property
	def MachineIp(self):
		return self.ownerComp.par.Machineip.eval()

	def UpdateAssetDirectories(self):
		'''
		updates the project paths for media and asset categories
		'''
		if hasattr(self.ownerComp.par, 'Rootmediafolder'):
			project.paths['Rootmedia'] = self.ownerComp.par.Rootmediafolder.val
		print('master class updated root media directory to ', project.paths['Rootmedia'])

	def SetupMachineId(self):
		# automated fetching of the machine's ip and hostname
		hostname = socket.gethostname()
		ip = socket.gethostbyname(hostname)

		if hasattr(self.ownerComp.par, 'Machineip'):
			self.ownerComp.par.Machineip.val = ip

		if hasattr(self.ownerComp.par, 'Machinename'):
			self.ownerComp.par.Machinename.val = hostname

		#determine the machine's role based on ip
		roleDict = {}
		for ipPar in self.ownerComp.customPages[2]:
			roleDict[ipPar.val] = ipPar.name 

		finalRole = "Role IP Error"	

		if hasattr(self.ownerComp.par, 'Machinerole'):
			if ip in roleDict.keys():
				finalRole = mainRolesDict[roleDict[ip]]
				
			self.ownerComp.par.Machinerole.val = finalRole


	def LoadConfig(self, machineRoleId = 1, runIdSetup = True):
		#mainConfigFile = self.configBase.op('mainConfigFile')
		mainConfig = self.configBase.op('mainConfigDat')
		#mainConfigFile.par.refreshpulse.pulse()

		#kick all config inputs
		for fOp in self.configBase.findChildren(tags = ['configFile']):
			fOp.par.refreshpulse.pulse()
		self.configBase.cook(force = True, recurse = True)	

		for entry in mainConfig.rows()[1:]:
			parName = entry[0].val
			val = entry[machineRoleId].val
			if hasattr(self.ownerComp.par, parName):
				print('setting project par: ', parName, ' to : ', val)
				targPar = getattr(self.ownerComp.par, parName)
				targPar.val = val

		if runIdSetup:
			self.SetupMachineId()


	
	def TogglePar(self, par):
		state = bool(par.eval())
		par.val = (bool(int(abs(1-state))))
		return par.val


