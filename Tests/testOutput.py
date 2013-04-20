#!/usr/bin/python

from lxml import etree
from subprocess import Popen, PIPE, STDOUT

import unittest

class OutputTests(unittest.TestCase):
	def setUp(self):
		print("")
	
	def validateXMLTree(self, input, expected):
		p = Popen(['../Build/blahtex', '--mathml', '--spacing', 'moderate'], stdout=PIPE, stdin=PIPE, stderr=STDOUT)
		
		p.stdin.write(input)
		p.stdin.close()
		p.wait()
		
		parser = etree.XMLParser(remove_blank_text=True)
		
		actual = p.stdout.read()
		
		rootNode = etree.XML(actual, parser=parser)
		expectedRootNode = etree.XML(expected, parser=parser)
		
		o1 = etree.tostring(rootNode)
		o2 = etree.tostring(expectedRootNode)
		
		self.assertEquals(o1, o2)


class TextTests(OutputTests):
	def testRawText(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>t</mi><mi>e</mi><mi>s</mi><mi>t</mi></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("test", output)
	
	def testRawText2(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>c</mi><mi>o</mi><mi>m</mi><mi>p</mi><mi>u</mi><mi>t</mi><mi>e</mi><mo stretchy="false">(</mo><mi>T</mi><mo stretchy="false">)</mo></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("compute(T)", output)


class ExpressionTests(OutputTests):
	def testExpr(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>n</mi><mo>=</mo><mn>1</mn></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("n = 1", output)
	
	def testExpr2(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>n</mi><mo>=</mo><mn>1</mn><mo>,</mo><mo>.</mo><mo>.</mo><mo>.</mo><mo>,</mo><mi>k</mi><mo>-</mo><mn>1</mn></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("n = 1, ... , k - 1", output)


class EquationTests(OutputTests):
	def testEq(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>n</mi><mo>=</mo><msup><mn>2</mn><mrow><mi>h</mi><mo>+</mo><mn>1</mn></mrow></msup><mo>-</mo><mn>1</mn></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("n = 2^{h+1} - 1", output)
	
	def testEq2(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>&#x3b8;</mi><mo stretchy="false">(</mo><mi>n</mi><mo stretchy="false">)</mo><mo>=</mo><mi>&#x3b8;</mi><mo stretchy="false">(</mo><msup><mn>2</mn><mrow><mi>h</mi><mo>+</mo><mn>1</mn></mrow></msup><mo>-</mo><mn>1</mn><mo stretchy="false">)</mo></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("\\theta(n) = \\theta(2^{h+1} - 1)", output)


class SymbolTests(OutputTests):
	def testTheta(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>&#x3b8;</mi><mo stretchy="false">(</mo><mi>n</mi><mo stretchy="false">)</mo></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("\\theta(n)", output)
	
	def testTheta2(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>&#x3b8;</mi><mo stretchy="false">(</mo><msup><mn>2</mn><mi>h</mi></msup><mo stretchy="false">)</mo></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("\\theta(2^h)", output)

if __name__ == '__main__':
	unittest.main()

