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
	
	def testExpr3(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>n</mi><mo>&#8800;</mo><mn>3</mn></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("n \\neq 3", output)
	
	def testExpr4(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>x</mi><mi>y</mi><mo>=</mo><mi>s</mi><mi>t</mi></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("xy = st", output)


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
	
	def testEq3(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>S</mi><mo>=</mo><mi>G</mi><mo>-</mo><mo stretchy="false">{</mo><mi>e</mi><mo stretchy="false">}</mo></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("S = G - \\{ e \\}", output)
	
	def testEqLong(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mn>2</mn><mo stretchy="false">(</mo><mi>k</mi><mo>-</mo><mn>1</mn><mo stretchy="false">)</mo><mo>-</mo><mn>1</mn><mo>+</mo><mn>2</mn><mo>=</mo><mn>2</mn><mi>k</mi><mo>-</mo><mn>2</mn><mo>-</mo><mn>1</mn><mo>+</mo><mn>2</mn><mo>=</mo><mn>2</mn><mi>k</mi><mo>-</mo><mn>1</mn></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("2(k - 1) - 1 + 2 = 2k - 2 - 1 + 2 = 2k - 1", output)

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


class VariableTests(OutputTests):
	def testVariables(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>x</mi><mo>,</mo><mi>y</mi><mo>,</mo><mi>s</mi><mo>,</mo><mi>t</mi><mo>,</mo><mi>m</mi><mo>,</mo><mi>n</mi><mo>&#x2208;</mo><mi mathvariant="double-struck">R</mi></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("x, y, s, t, m, n \\in \\mathbb{R}", output)
	
	def testVariables2(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>f</mi><mo>:</mo><mi mathvariant="double-struck">N</mi><mo>&#x2192;</mo><mi mathvariant="double-struck">N</mi></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("f : \\mathbb{N} \\rightarrow \\mathbb{N}", output)
		
	def testVariables3(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>g</mi><mo>:</mo><msup><mi mathvariant="double-struck">N</mi><mn>2</mn></msup><mo>&#x2192;</mo><mi mathvariant="double-struck">Z</mi></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("g : \\mathbb{N}^2 \\rightarrow \\mathbb{Z}", output)

class FunctionTests(OutputTests):
	def testFunction(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<mrow><mi>g</mi><mo stretchy="false">(</mo><mi>m</mi><mo>,</mo><mi>n</mi><mo stretchy="false">)</mo><mo>=</mo><mo stretchy="false">(</mo><mn>2</mn><mo>-</mo><mi>n</mi><mo stretchy="false">)</mo><mi>f</mi><mo stretchy="false">(</mo><mi>m</mi><mo stretchy="false">)</mo></mrow>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("g(m,n) = (2 - n) f(m)", output)

class ScriptTests(OutputTests):
	def testSubscript(self):
		output = """
			<blahtex>
			<mathml>
			<markup>
			<msub><mi>W</mi><mn>3</mn></msub>
			</markup>
			</mathml>
			</blahtex>
		"""
		
		self.validateXMLTree("W_3", output)

if __name__ == '__main__':
	unittest.main()

