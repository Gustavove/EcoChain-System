import ipfshttpclient
import json

# Share TCP connections until the client session is closed
class IPFSconnection:
	def __init__(self, max_cont):
		self._client = ipfshttpclient.connect(session=True)
		self.contData = 0
		self.maxContData = max_cont
		self.dic_to_send = {}

	def addData(self, _data):
		cid = ""
		if(self.contData < self.maxContData):
			self.dic_to_send[self.contData] = _data
			self.contData += 1
			print('dato subido: ' + str(self.contData))
			if self.contData == self.maxContData:
				cid = self._client.add_json(self.dic_to_send)
				print('data uploaded: ' + str(self.dic_to_send))
				self.dic_to_send.clear()
				self.contData = 0
		return cid
	def getData(self, cid):
		get_data = self._client.get_json(cid)
		return get_data
	def close(self):  # Call this when your done
		self._client.close()