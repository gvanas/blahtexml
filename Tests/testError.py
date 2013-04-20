#!/usr/bin/python

from subprocess import Popen, PIPE, STDOUT
import unittest
import xml.etree.ElementTree as ET

class ErrorTests(unittest.TestCase):
	def setUp(self):
		print("")
	
	def validateError(self, input, errorId, startPos, length):
		p = Popen(['/Users/Matt/Projects/blahtexml/Build/blahtex', '--mathml'], stdout=PIPE, stdin=PIPE, stderr=STDOUT)
		
		p.stdin.write(input)
		p.stdin.close()
		p.wait()
		
		rootNode = ET.fromstring(p.stdout.read())
		
		errorNode = rootNode.find("error")
		
		self.assertEqual(errorNode.find("id").text, errorId)
		self.assertEqual(int(errorNode.find("startPos").text), startPos)
		self.assertEqual(int(errorNode.find("length").text), length)


class BraceErrorTests(ErrorTests):
	def testOpenBrace(self):
		self.validateError("2^{5", "UnmatchedOpenBrace", 2, 1)
		self.validateError("4^{6 * 2^{5", "UnmatchedOpenBrace", 2, 1)
		self.validateError("4^{6} * 2^{5", "UnmatchedOpenBrace", 10, 1)
		self.validateError("2^{2{5}", "UnmatchedOpenBrace", 2, 1)
	
	def testCloseBrace(self):
		self.validateError("2^5}", "UnmatchedCloseBrace", 3, 1)


if __name__ == '__main__':
	unittest.main()

