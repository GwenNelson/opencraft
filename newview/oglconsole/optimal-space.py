import sys
mode = 'default'
try:
    if sys.argv[1] == '-c':
        mode = 'custom'
except: pass

if mode == 'custom':
    cw = int(raw_input("Please input character width in pixels: "))
    ch = int(raw_input("Please input character height in pixels: "))
    cn = int(raw_input("Please input number of total characters: "))
elif mode == 'default':
    cw = 6
    ch = 13
    cn = 94
    print "Character dimensions: 6x13 (invoke with -c option to change)"
    print "Number of characters: 94"


GridOrientations = []
rows = 0

while 1:
    # Increment number of rows
    rows += 1

    # Calculate number of columns and wasted character spaces
    columns = cn / rows
    waste = cn - rows * columns
    if waste:
        columns += 1
        waste = rows * columns - cn

    # Bail out once we hit the last combination
    if columns < rows:
        print "Finished calculating grid orientations"
        break

    # Construct results for storage
    GridOrientations.append((rows, columns, waste, waste * cw * ch))

# Finished calculating grid orientations

def PrintGridOrientation(o):
    rows, columns, waste_c, waste = o
    print "Characters: %i\t%i\t(rows, columns)" %(rows, columns)
    print "Waste:      %i\t%i\t(characters, pixels)" %(waste_c,waste)

print

for each in GridOrientations:
    PrintGridOrientation(each)
    print

#######################################################################
# Now we'll calculate the pixel waste for power-of-two texture maps

PowerOrientations = []

for Orientation in GridOrientations:

    # Use these results twice, reversing them once
    for polarity in range(2):
        if polarity:
            Orientation = Orientation[1], Orientation[0], Orientation[2], \
            Orientation[3]

        # Fetch results of last calculations
        rows, columns, waste_c, waste = Orientation
        rows *= ch
        columns *= cw

        # Calculate how many pixels have been used before power-of-two
        used = rows * columns - waste

        # Calculate power-of-twos
        p = 1
        while 2 ** p < rows: p += 1
        rows = 2 ** p

        p = 1
        while 2 ** p < columns: p += 1
        columns = 2 ** p
        
        # Calculate waste
        total = rows * columns
        waste = total - used

        PowerOrientations.append((Orientation, rows, columns, total, used, waste))

def PrintPowerOrientation(o):
    grido, rows, cols, total, used, waste = o

    PrintGridOrientation(grido)
    print "Pixels:     %i\tx %i\t(width by height)" % (cols, rows)
    print "Use/Waste:  %i\t%i\t(total %i)" % (used, waste, total)

print "Done calculating the power-of-two textures"
print
for each in PowerOrientations:
    PrintPowerOrientation(each)
    print

