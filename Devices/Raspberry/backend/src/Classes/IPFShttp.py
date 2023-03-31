import ipfshttpclient
from Modules import temp_sensors_data
import json

temp_sensors_data.init()

class IPFSconnection:
	def __init__(self, max_cont):
		self._client = ipfshttpclient.connect(session=True)
		self.maxContData = max_cont-1

	def addData(self, _data, sensor_id):
		cid = ""
		contData = int(temp_sensors_data.get_last_data_id(sensor_id))
		print(contData)
		print(str(type(contData)))
		if(contData <= self.maxContData):
			temp_sensors_data.save_sensor_data(sensor_id, _data)
			print('dato subido: ' + str(contData))
			if contData == self.maxContData:
				info_to_add = json.loads(temp_sensors_data.get_sensor_data(sensor_id))
				cid = self._client.add_json(info_to_add)
				print('data uploaded: ' + str(temp_sensors_data.get_sensor_data(sensor_id)))
				temp_sensors_data.delete_sensor_data(sensor_id)
		return cid
	def getData(self, cid):
		get_data = self._client.get_json(cid)
		return get_data
	def close(self):  # Call this when your done
		self._client.close()