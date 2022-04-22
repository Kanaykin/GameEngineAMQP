import sys
import subprocess
import unittest
from subprocess import Popen

BIN_DIR = '../build/bin/'

def multy_proc_tests(cmds, checkResultFunc):
	print("multy_proc_tests ")
	procs = [ subprocess.Popen(i, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) for i in cmds ]
	for p in procs:
		p.wait()

	full_out = ""
	for p in procs:
		while True:
			line = p.stdout.readline()
			if not line:
				break
			#the real code does filtering here
			# print(str(line))
			full_out = full_out + str(line)

	checkResultFunc(full_out)
	

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
		checkResultFunc = lambda outstr: (
			self.assertFalse(outstr.find("ProtonConsumer::on_message [consumer]") == -1))

		multy_proc_tests(cmds, checkResultFunc)


	def test_broker_client_server(self):
		print("test_broker_client_server ")

		cmds = [ BIN_DIR + "broker_exchange", BIN_DIR + "broker_producer", BIN_DIR + "broker_consumer"]
		
		checkResultFunc = lambda outstr: (self.assertFalse(outstr.find("ProtonConsumer::on_message [customer1]") == -1), 
		self.assertFalse(outstr.find("ProtonConsumer::on_message [customer2]") == -1))
		
		multy_proc_tests(cmds, checkResultFunc)


if __name__ == '__main__':
    unittest.main()