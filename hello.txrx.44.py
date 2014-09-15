#
# hello.txrx.44.py
#
# receive and display transmit-receive step response
# hello.step.44.py serial_port
#
# Neil Gershenfeld
# CBA MIT 11/6/11
#
# (c) Massachusetts Institute of Technology 2011
# Permission granted for experimental and personal use;
# license for commercial sale available from MIT
#

from Tkinter import *
import serial

WINDOW = 600 # window size
eps = 0.75 # filter fraction
filt = 0.0 # filtered value
padnumber = 0

def idle(parent,canvas):
   global filt, eps
   #
   # idle routine
   #
   byte2 = 0
   byte3 = 0
   byte4 = 0
   ser.flush()
   #
   # find framing 
   #
   while 1:
      byte1 = byte2
      byte2 = byte3
      byte3 = byte4
      byte4 = ord(ser.read())
      if ((byte1 == 1) & (byte2 == 2) & (byte3 == 3) & (byte4 == 4)):
         break
   #
   # read and plot
   #
   padnumber = int(ord(ser.read())) - 1 
   #print "-----------"
   #print "padnumber: %d"%padnumber

   up_low = ord(ser.read())
   #print "up_low: %d"%up_low
   up_high = ord(ser.read())
   #print "up_high: %d"%up_high
   down_low = ord(ser.read())
   #print "down_low: %d"%down_low
   down_high = ord(ser.read())
   #print "down_high: %d"%down_high
   up_value = 256*up_high + up_low
   down_value = 256*down_high + down_low
   value = (up_value - down_value)
   filt = (1-eps)*filt + eps*value
   x = int(.2*WINDOW + (.9-.2)*WINDOW*filt/10000.0)
   
   print padnumber, value
			
   if(padnumber == 0):
      canvas.itemconfigure("pad1text",text="%.1f"%filt)
      canvas.coords('pad1fillrect',.2*WINDOW,.05*WINDOW+.25*WINDOW*padnumber,x,.2*WINDOW+.25*WINDOW*padnumber)
      canvas.coords('pad1emptyrect',x,.05*WINDOW+.25*WINDOW*padnumber,.9*WINDOW,.2*WINDOW+.25*WINDOW*padnumber)
   elif (padnumber == 1):
      canvas.itemconfigure("pad2text",text="%.1f"%filt)
      canvas.coords('pad2fillrect',.2*WINDOW,.05*WINDOW+.25*WINDOW*padnumber,x,.2*WINDOW+.25*WINDOW*padnumber)
      canvas.coords('pad2emptyrect',x,.05*WINDOW+.25*WINDOW*padnumber,.9*WINDOW,.2*WINDOW+.25*WINDOW*padnumber)
   elif (padnumber == 2):
      canvas.itemconfigure("pad3text",text="%.1f"%filt)
      canvas.coords('pad3fillrect',.2*WINDOW,.05*WINDOW+.25*WINDOW*padnumber,x,.2*WINDOW+.25*WINDOW*padnumber)
      canvas.coords('pad3emptyrect',x,.05*WINDOW+.25*WINDOW*padnumber,.9*WINDOW,.2*WINDOW+.25*WINDOW*padnumber)
   elif (padnumber == 3):
      canvas.itemconfigure("pad4text",text="%.1f"%filt)
      canvas.coords('pad4fillrect',.2*WINDOW,.05*WINDOW+.25*WINDOW*padnumber,x,.2*WINDOW+.25*WINDOW*padnumber)
      canvas.coords('pad4emptyrect',x,.05*WINDOW+.25*WINDOW*padnumber,.9*WINDOW,.2*WINDOW+.25*WINDOW*padnumber)

   canvas.update()
   parent.after_idle(idle,parent,canvas)

#
#  check command line arguments
#
if (len(sys.argv) != 2):
   print "command line: hello.txrx.44.py serial_port"
   sys.exit()
port = sys.argv[1]
#
# open serial port
#
ser = serial.Serial(port,115200)
ser.setDTR()
#
# set up GUI
#
root = Tk()
root.title('hello.txrx.44.py (q to exit)')
root.bind('q','exit')
canvas = Canvas(root, width=WINDOW, height=WINDOW, background='white')
#
canvas.create_text(.1*WINDOW,.125*WINDOW+.125*WINDOW*0,text="1",font=("Helvetica", 24),tags="pad1text",fill="black")
canvas.create_rectangle(.2*WINDOW,.05*WINDOW+.25*WINDOW*0,.3*WINDOW,.2*WINDOW+.25*WINDOW*0, tags='pad1fillrect', fill='red')
canvas.create_rectangle(.3*WINDOW,.05*WINDOW,.9*WINDOW,.2*WINDOW, tags='pad1emptyrect', fill='white')

canvas.create_text(.1*WINDOW,.125*WINDOW+.125*WINDOW*2*1,text="1",font=("Helvetica", 24),tags="pad2text",fill="black")
canvas.create_rectangle(.2*WINDOW,.05*WINDOW+.25*WINDOW*1,.3*WINDOW,.2*WINDOW+.25*WINDOW*1, tags='pad2fillrect', fill='yellow')
canvas.create_rectangle(.3*WINDOW,.05*WINDOW+.25*WINDOW*1,.9*WINDOW,.2*WINDOW+.25*WINDOW*1, tags='pad2emptyrect', fill='white')

canvas.create_text(.1*WINDOW,.125*WINDOW+.125*WINDOW*2*2,text="1",font=("Helvetica", 24),tags="pad3text",fill="black")
canvas.create_rectangle(.2*WINDOW,.05*WINDOW+.25*WINDOW*2,.3*WINDOW,.2*WINDOW+.25*WINDOW*2, tags='pad3fillrect', fill='grey')
canvas.create_rectangle(.3*WINDOW,.05*WINDOW+.25*WINDOW*2,.9*WINDOW,.2*WINDOW+.25*WINDOW*2, tags='pad3emptyrect', fill='white')

canvas.create_text(.1*WINDOW,.125*WINDOW+.125*WINDOW*2*3,text="1",font=("Helvetica", 24),tags="pad4text",fill="black")
canvas.create_rectangle(.2*WINDOW,.05*WINDOW+.25*WINDOW*3,.3*WINDOW,.2*WINDOW+.25*WINDOW*3, tags='pad4fillrect', fill='black')
canvas.create_rectangle(.3*WINDOW,.05*WINDOW+.25*WINDOW*3,.9*WINDOW,.2*WINDOW+.25*WINDOW*3, tags='pad4emptyrect', fill='white')

canvas.pack()
#
# start idle loop
#
root.after(100,idle,root,canvas)
root.mainloop()
