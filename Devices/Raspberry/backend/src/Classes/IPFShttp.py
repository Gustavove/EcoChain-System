import ipfshttpclient

# Share TCP connections until the client session is closed
class IPFSconnection:
	def __init__(self):
		self._client = ipfshttpclient.connect(session=True)
		self.contData = 0

	def do_something(self):
		hash = self._client.add('test.txt')['Hash']
		print(self._client.stat(hash))

	def addData(self, _data):
		cid = self._client.add_json(_data)
		return cid
	def getData(self, cid):
		return self._client.get_json(cid)

	def close(self):  # Call this when your done
		self._client.close()