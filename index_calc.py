#!/usr/bin/python

#================= Input =================

# Neighbor's values
x = 4   # relative x
y = -4  # relative y
t = 180 # relative angle
# Database configuration values
MaxX = 4   # Max X value
MaxY = 4   # Max Y value
MaxT = 359 # Max angle value
ppg = 1    # Pixels per group
dpg = 90   # Degrees per group

#=========================================

#Do not change the code below

XRangeSize = (2 * MaxX) + 1
XGroups = XRangeSize / ppg
if XRangeSize % ppg > 0:
    XGroups += 1

YRangeSize = (2 * MaxY) + 1
YGroups = YRangeSize / ppg
if YRangeSize % ppg > 0:
    YGroups += 1

TRangeSize = MaxT + 1
TGroups = TRangeSize / dpg
if TRangeSize % dpg > 0:
    TGroups += 1

print (XGroups, YGroups, TGroups)

NumGroups = XGroups * YGroups * TGroups

XBlockSize = YGroups * TGroups
YBlockSize = TGroups

XComp = (XBlockSize * ((MaxX + x) / ppg))
YComp = (YBlockSize * ((MaxY + y) / ppg))
TComp = (t / dpg)

GroupIndex = XComp + YComp + TComp

print "NumGroups: ", NumGroups
print "GroupIndex: ", GroupIndex
