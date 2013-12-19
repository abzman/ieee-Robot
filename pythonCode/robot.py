from SimpleCV import *
import serial
import time
import atexit

ser = serial.Serial('/dev/ttyUSB0', 9600)

def drive(speed='', turn='', power=''):
	buf = "SPEED %s\r\nTURN %s\r\n%s\r\n" % (str(speed), str(turn), power)
	print buf;
	ser.write(buf)

def good(var):
	return (700 <= var and var <= 1700)

def main():
	historyIndex = 0
	historyLength = 5
	averagingSize = 31

	leftHistory = [0]*historyLength
	rightHistory = [0]*historyLength
	centerHistory = [0]*historyLength

	k = Kinect()
	disp = Display()
	while disp.isNotDone():
		try:
			d = k.getDepth()
			#img = k.getImage()
			depthMatrix = k.getDepthMatrix()
			resolution = (len(depthMatrix[0]), len(depthMatrix))
			
			d.dl().rectangle((resolution[0]-averagingSize-10, resolution[1]/2 - int((averagingSize-1)/2)), (averagingSize,averagingSize), Color.GREEN)	
			d.dl().rectangle((0, resolution[1]/2 - int((averagingSize-1)/2)), (averagingSize,averagingSize), Color.BLUE)	
			d.dl().rectangle((resolution[0]/2 - int((averagingSize-1)/2), resolution[1]/2 - int((averagingSize-1)/2)), (averagingSize,averagingSize), Color.RED)
	
			#Read depth map for left, right, and center
			leftDepth = 0
			for i in range(averagingSize):
				for j in range(-int(averagingSize/2)+1, int(averagingSize/2)):
					leftDepth += depthMatrix[resolution[1]/2 + j][i]
			leftDepth /= averagingSize ** 2
				
			rightDepth = 0
			for i in range(averagingSize):
				for j in range(-int(averagingSize/2)+1, int(averagingSize/2)):
					rightDepth += depthMatrix[resolution[1]/2 + j][resolution[0] - i - 11]
			rightDepth /= averagingSize ** 2

			centerDepth = 0
			for i in range(averagingSize):
				for j in range(-int(averagingSize/2)+1, int(averagingSize/2)):
					centerDepth += depthMatrix[resolution[1]/2 + j][resolution[0]/2 + i]
			centerDepth /= averagingSize ** 2


			#Average n last readings
			leftHistory[historyIndex] = leftDepth
			leftDepth = sum(leftHistory) / historyLength

			rightHistory[historyIndex] = rightDepth
			rightDepth = sum(rightHistory) / historyLength

			centerHistory[historyIndex] = centerDepth
			centerDepth = sum(centerHistory) / historyLength

			#increment index and roll over
			historyIndex = (historyIndex + 1) % historyLength 
			
			#print leftDepth,"\t-\t", centerDepth,"\t-\t",rightDepth
			
			if good(leftDepth) and good(rightDepth) and good(centerDepth):
				drive(1023) # forward
			elif good(leftDepth):
				drive(512, 256) # spin left
			elif good(rightDepth):
				drive(512,756) # spin right
			else:
				drive(0) # reverse
				time.sleep(1.5) #delay next move
				drive(512,1023) #spin right
				time.sleep(1) #delay next move
				
			d.show()
			#img.sideBySide(d).show()
		except KeyboardInterrupt:
			break

if __name__ == "__main__":
	atexit.register(drive, 512, 512, 'POWER 0')
	time.sleep(2)
	ser.write("POWER 1\r\n")
	main()
