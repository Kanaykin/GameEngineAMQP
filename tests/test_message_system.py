import sys
import subprocess
import unittest
from subprocess import Popen

BIN_DIR = '../build/bin/'

class TestWeekendDays(unittest.TestCase):

	def test_broker(self):
		
		print("test_broker ")
		result = subprocess.run(BIN_DIR + 'simple_broker', capture_output=True)

		self.assertFalse(str(result.stdout).find("ProtonConsumer::on_message [customer1]") == -1)
		self.assertFalse(str(result.stdout).find("ProtonConsumer::on_message [customer2]") == -1)

		self.assertEqual(result.returncode, 0)

	def test_peer_to_peer(self):
		print("test_peer_to_peer ")
		result = subprocess.run(BIN_DIR + 'peer-to-peer', capture_output=True)

		self.assertFalse(str(result.stdout).find("ProtonConsumer::on_message [consumer]") == -1)

		self.assertEqual(result.returncode, 0)

	def test_peer_to_peer_client_server(self):
		print("test_peer_to_peer_client_server ")

		cmds = [BIN_DIR + "peer-to-peer-server", BIN_DIR + "peer-to-peer-client"]

		procs = [ subprocess.Popen(i, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) for i in cmds ]
		for p in procs:
			p.wait()

		full_out = ""
		for p in procs:
			print("str(out) =========== ")
			out, err = p.communicate()
			print(str(out))
			full_out = full_out + str(out)
		
		self.assertFalse(full_out.find("ProtonConsumer::on_message [consumer]") == -1)

	def test_broker_client_server(self):
		print("test_broker_client_server ")

		cmds = [ BIN_DIR + "broker_exchange", BIN_DIR + "broker_producer", BIN_DIR + "broker_consumer"]
		procs = [ subprocess.Popen(i, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) for i in cmds ]
		for p in procs:
			p.wait()

		full_out = ""
		for p in procs:
			# print("str(out) =========== ")
			# out, err = p.communicate()
			# print(str(out))
			while True:
				line = p.stdout.readline()
				if not line:
					break
				#the real code does filtering here
				print(str(line))
				full_out = full_out + str(line)

		self.assertFalse(full_out.find("ProtonConsumer::on_message [customer1]") == -1)
		self.assertFalse(full_out.find("ProtonConsumer::on_message [customer2]") == -1)

		# while True:
		# 	line = full_out.readline()
		# 	if not line:
		# 		break
		# 	#the real code does filtering here
		# 	print("test:" + line.rstrip())

	# 	result = subprocess.run(command)
		# self.assertFalse(str(result.stdout).find("ProtonConsumer::on_message [consumer]") == -1)

		# self.assertEqual(result.returncode, 0)


if __name__ == '__main__':
    unittest.main()