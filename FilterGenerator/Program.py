# Copyright 2011, Mattaeus G. Chajdas. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
# 
#    1. Redistributions of source code must retain the above copyright notice, this list of
# 	  conditions and the following disclaimer.
# 
#    2. Redistributions in binary form must reproduce the above copyright notice, this list
# 	  of conditions and the following disclaimer in the documentation and/or other materials
# 	  provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY MATTHAEUS G. CHAJDAS ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHAEUS G. CHAJDAS OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# The views and conclusions contained in the software and documentation are those of the
# authors and should not be interpreted as representing official policies, either expressed
# or implied, of Mattaeus G. Chajdas.

# Tested with Python 3.2

def sinc (x):
	import math
	if x == 0:
		return 1
	else:
		return math.sin (math.pi * x) / (math.pi * x)

def Lanczos1D (x, a):
	import math
	if -a < x < a:
		return sinc (x) * sinc (x/a)
	else:
		return 0

def Gaussian1D (x, stdDev = 1.0):
	import math
	return math.exp (-x**2 / (2 * stdDev**2)) / math.sqrt (2 * math.pi * stdDev**2)

def Gaussian2D (x, y, stdDev = 1.0):
	return Gaussian1D (x, stdDev) * Gaussian1D (y, stdDev)

def Lanczos2D (x, y, a):
	return Lanczos1D (x, a) * Lanczos1D (y, a)

def Box2D (x, y, a):
	return 1.0

MSAAPatterns = {1 : [(0, 0,)],
				2 : [(0.25, 0.25,),         (-0.25, -0.25,)],
				4 : [(-0.125, -0.375,),     (0.375, -0.125,), 
					 (-0.375, 0.125,),      (0.125, 0.375,)],
				8: [(0.0625, -0.1875,),     (-0.0625,  0.1875,), 
					(0.3125, 0.0625),       (-0.1875, -0.3125,),
					(-0.3125,  0.3125,),    (-0.4375, -0.0625,),
					(0.1875,  0.4375,),     (0.4375, -0.4375,)]
				}

cutoff = 1.0e-3

def WriteFilterWeights (filter, sampleCount, width):
	import math
	print ("#if MSAA_SAMPLES == {}".format (sampleCount))
	print ("float4 {} (int2 pos)\n{}".format (filter.__name__, '{'))
	n = 0
	culled = 0
	print ("\tfloat4 result = float4(0,0,0,0);\n")
	totalWeight = 0.0
	iWidth = int(math.ceil(width))
	for y in range (-iWidth, iWidth+1):
		for x in range (-iWidth, iWidth+1):
			print ("\t// {x}, {y}".format (x=x, y=y))
			for sample in range (sampleCount):
				samplePosition = MSAAPatterns [sampleCount][sample]
				weight = filter (samplePosition[0]+x, samplePosition[1]+y, width)
				if abs (weight) > cutoff:
					if sampleCount > 1:
						print ("\tresult += {w} * source.Load (pos, {s}, int2({x}, {y}));".format (
								n = n, x = x, y = y, s = sample, w = weight))
					else:
						print ("\tresult += {w} * source.Load (int3(pos,0), int2({x}, {y}));".format (
								n = n, x = x, y = y, w = weight))
					n += 1
					totalWeight += weight
				else:
					culled += 1
	print ("\n\treturn result / {}; // {} ({}) samples".format (totalWeight, n, n+culled))
	print ("}\n")
	print ("#endif\n\n")

for i in [1,2,4,8]:
	# Might want to adjust the filter size for Lanczos
	WriteFilterWeights (Lanczos2D, i, 2)
	WriteFilterWeights (Gaussian2D, i, 0.5)
	WriteFilterWeights (Box2D, i, 0)
