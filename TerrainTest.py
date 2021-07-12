import pygame, sys, random
pygame.init()

WX = 800
WY = 600
WINDOW = pygame.display.set_mode((WX, WY), 0, 32)
pygame.display.set_caption("Microcosm Terrain Tests")
CHRONO = pygame.time.Clock()
ZOOM = 6

# In Test of Time, default world sizes are 40x50, 50x80, and 75x120

WORLD_X = 120
WORLD_Y = 75
X_OFFSET = 0
Y_OFFSET = 0
WORLD = []

WATER_LEVEL = 0
PRESSURE_POINT_ODDS = 50
MIN_PRESSURE = 2
MAX_PRESSURE = 8
POLAR_OCEAN_SIZE = 3
POLAR_DROPOFF = 2
POLAR_ODDS = 100
POLAR_HEIGHT = 50
RIVER_ODDS = 50
RIVER_MIN_STRENGTH = 3
RIVER_MAX_STRENGTH = 7
RIVER_BRANCH_ODDS = 50
MTN_THRESHOLD = 4
MTN_ODDS = 3
MTN_MOD = 2



class Tile:
    def __init__(self, tileType='G', height=0):
        self.height = height
        self.changeTileType(tileType)

    def changeTileType(self, tileType):
        self.tileType = tileType
        if tileType == 'G':  # Grassland
            self.color = (0, 200, 0)
        elif tileType == 'M':  # Mountain
            self.color = (127, 127, 127)
        elif tileType == 'D':  # Desert
            self.color = (255, 255, 0)
        elif tileType == 'T':  # Tundra
            self.color = (255, 255, 255)
        elif tileType == 'F':  # Forest
            self.color = (0, 100, 0)
        elif tileType == 'S':  # Swamp
            self.color = (0, 0, 0)
        elif tileType == 'H':  # Hills
            self.color = (100, 100, 0)
        elif tileType == 'R':  # River
            self.color = (0, 127, 127)
        else:
            self.color = (255, 0, 255)

def resizeMap():
    WORLD.clear()
    for row in range(WORLD_Y):
        WORLD.append([])
        for col in range(WORLD_X):
            WORLD[row].append(Tile('G', 0))

def generateHeight1():
    # Simple generator, just pick a random height for each tile between 1 to 4
    for row in range(WORLD_Y):
        for col in range(WORLD_X):
            WORLD[row][col].height = random.randint(0, 4)

def generateHeight2():
    # Each tile has a probability of containing a pressure point.
    for row in range(WORLD_Y):
        for col in range(WORLD_X):
            dieRoll = random.randint(1, PRESSURE_POINT_ODDS)
            if (row + POLAR_OCEAN_SIZE >= WORLD_Y) or (row - POLAR_OCEAN_SIZE <= 0):  # If we're close to the north or south pole
                dieRoll = random.randint(1, POLAR_ODDS)  # Use the polar odds
            if dieRoll == 1:  # There's a pressure point
                pressure = random.randint(1, MAX_PRESSURE)
                # Go in a simple radius around the pressure point
                for y in range(row-pressure, row+pressure+1):
                    if y >= 0 and y < WORLD_Y:  # Don't go over the north/south edge of the map, even with modulo.
                        width = pressure - abs(y - row)
                        for x in range(col-width, col+width+1):
                            prevHeight = WORLD[y][x % WORLD_X].height
                            WORLD[y][x % WORLD_X].height += pressure - (abs(y-row) + abs(x-col))
                            if (y + POLAR_OCEAN_SIZE >= WORLD_Y) or (y - POLAR_OCEAN_SIZE <= 0):
                                WORLD[y][x % WORLD_X].height -= POLAR_DROPOFF
                            if prevHeight > MTN_THRESHOLD:
                                mtnDieRoll = random.randint(1, MTN_ODDS)
                                if mtnDieRoll <= 1:
                                    WORLD[y][x % WORLD_X].changeTileType('M')
    # Finally, add a ring of tiles around the north and south poles
    for col in range(WORLD_X):
        WORLD[0][col].height = POLAR_HEIGHT
        WORLD[0][col].changeTileType('T')
        WORLD[WORLD_Y-1][col].height = POLAR_HEIGHT
        WORLD[WORLD_Y-1][col].changeTileType('T')

def generateRivers():
    sourceDirection = 0  # Are we going uphill from water or downhill from a mountain?
    for row in range(1, WORLD_Y-1):
        for col in range(WORLD_X):
            # Get the tile and surrounding tiles
            tile = WORLD[row][col]
            nTile = WORLD[row-1][col]
            sTile = WORLD[row+1][col]
            eTile = WORLD[row][(col+1) % WORLD_X]
            wTile = WORLD[row][(col-1) % WORLD_X]
            if tile.tileType == 'G':  # Only change grasslands, the default
                for surroundingTile in [nTile, sTile, eTile, wTile]:
                    if (surroundingTile.tileType == 'M') or (surroundingTile.height <= WATER_LEVEL):
                        if surroundingTile.tileType == 'M':
                            sourceDirection = -1  # Go downhill
                        elif surroundingTile.height <= WATER_LEVEL:
                            sourceDirection = 1  # Go uphill
                        dieRoll = random.randint(1, RIVER_ODDS)
                        if dieRoll == 1:  # Start a river
                            strength = random.randint(RIVER_MIN_STRENGTH, RIVER_MAX_STRENGTH) * sourceDirection
                            generateRiversHelper(row, col, strength)  # Recurse

                        break  # Once we've found one qualifying tile, do not repeat for the other surrounding tiles

def generateRiversHelper(row, col, strength):
    # Recursive component of generateRivers().  Probably needs reworked heavily for C
    if (strength != 0):  # Base case
        tile = WORLD[row][col]
        tile.changeTileType('R')
        nTile = (row-1, col)
        sTile = (row+1, col)
        eTile = (row, (col+1) % WORLD_X)
        wTile = (row, (col-1) % WORLD_X)
        options = [nTile, sTile, eTile, wTile]
        # Pick a direction at random and see if it works.
        for i in range(4):
            random.shuffle(options)
            choiceLocation = options.pop()
            choice = WORLD[choiceLocation[0]][choiceLocation[1]]
            if choice.tileType == 'G' and (((strength < 0) and (choice.height <= tile.height)) or ((strength > 0) and (choice.height >= tile.height))):  # If the tile works
                if (strength < 0) and (choice.height <= tile.height):  # Negative strength: downhill
                    generateRiversHelper(choiceLocation[0], choiceLocation[1], strength+1)
                elif (strength > 0) and (choice.height >= tile.height):  # Positive strength: uphill
                    generateRiversHelper(choiceLocation[0], choiceLocation[1], strength-1)

                # See if we branch
                dieRoll = random.randint(1, RIVER_BRANCH_ODDS)
                if (dieRoll != 1):
                    break
                #break
    
    

def generateTypes1():
    # Do something, Taipu
    for row in range(1, WORLD_Y-1):
        for col in range(WORLD_X):
            # Get the tile and surrounding tiles
            tile = WORLD[row][col]
            nTile = WORLD[row-1][col]
            sTile = WORLD[row+1][col]
            eTile = WORLD[row][(col+1) % WORLD_X]
            wTile = WORLD[row][(col-1) % WORLD_X]
            if tile.tileType == 'G':  # Only change grasslands, the default
                tileTypes = ['G', 'G', 'F', 'S', 'H', 'M']
                tileTypeOdds = [8, 20, 16, 8, 10, 2]
                if (row < 10) or (row > WORLD_Y - 10):  # If we're within 10 tiles of the poles (we could use percentages instead but hopefully this works for demonstration)
                    tileTypes[0] = 'T'  # Use Tundra
                elif (row > (WORLD_Y / 2) - 10) and (row < (WORLD_Y / 2) + 10):  # If we're within 10 tiles of the equator
                    tileTypes[0] = 'D'  # Use Deserts
                # Adjust the odds based on surrounding tiles
                for surroundingTile in [nTile, sTile, eTile, wTile]:  # We can use more primitive logic if necessary, but this is readable
                    if (surroundingTile.tileType == tileTypes[0]):  # Desert/Tundra
                        tileTypeOdds[1] -= 1
                        tileTypeOdds[0] += 1
                    elif (surroundingTile.tileType == 'F'):
                        tileTypeOdds[1] -= 1
                        tileTypeOdds[2] += 1
                    elif (surroundingTile.tileType == 'S'):
                        tileTypeOdds[1] -= 1
                        tileTypeOdds[3] += 1
                    elif (surroundingTile.tileType == 'H') or (surroundingTile.tileType == 'M'):
                        tileTypeOdds[1] -= 1
                        tileTypeOdds[4] += 1
                    elif (surroundingTile.tileType == 'R') or (surroundingTile.height <= WATER_LEVEL):
                        tileTypeOdds[0] -= 2
                        tileTypeOdds[2] += 1
                        tileTypeOdds[3] += 1
                dieRoll = random.randint(1, sum(tileTypeOdds))  # Do a die roll.  Replace the sum() with a precalculated sum if needed
                # Based on that die roll, select the tile type
                total = 0
                tileType = 'G'
                for i in range(len(tileTypes)):
                    if dieRoll <= (total + tileTypeOdds[i]):
                        tileType = tileTypes[i]
                        break
                    total += tileTypeOdds[i]
                # Finally, change the tile's type accordingly
                tile.changeTileType(tileType)

def render(xOffset, yOffset):
    WINDOW.fill((0,0,0))
    for y in range(WORLD_Y):
        for x in range(WORLD_X):
            col = (x + xOffset) % WORLD_X
            row = (y + yOffset) % WORLD_Y
            # Draw onto the window
            tile = WORLD[row][col]
            rect = pygame.Rect(x*ZOOM, y*ZOOM, ZOOM, ZOOM)
            color = (0, 0, 127)
            if tile.height > WATER_LEVEL:
                color = tile.color
            pygame.draw.rect(WINDOW, color, rect)

def intInput(prompt):
    while True:
        userInput = input(prompt)
        try:
            number = int(userInput)
            return number
        except Exception:
            print("Not a number")
        

def main():
    global WATER_LEVEL, WORLD_X, WORLD_Y, PRESSURE_POINT_ODDS, MIN_PRESSURE, MAX_PRESSURE
    xOffset = 0
    yOffset = 0
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == pygame.KEYUP:
                if event.key == ord('r'):  # Reset
                    resizeMap()
                    generateHeight2()
                    generateRivers()
                    generateTypes1()
                elif event.key == ord('d'):  # Dimensions
                    WORLD_X = intInput("WORLD_X: ")
                    WORLD_Y = intInput("WORLD_Y: ")
                elif event.key == ord('p'):  # Pressure
                    PRESSURE_POINT_ODDS = intInput("PRESSURE_POINT_ODDS: ")
                    MIN_PRESSURE = intInput("MIN_PRESSURE")
                    MAX_PRESSURE = intInput("MAX_PRESSURE")
                elif event.key == ord('='):  # Increase water level
                    WATER_LEVEL += 1
                    print("WATER_LEVEL", WATER_LEVEL)
                elif event.key == ord('-'):  # Decrease water level
                    if WATER_LEVEL > 0:
                        WATER_LEVEL -= 1
                    print("WATER_LEVEL", WATER_LEVEL)
                #elif event.key == pygame.K_UP:
                #    yOffset = (yOffset - 1) % WORLD_Y
                #elif event.key == pygame.K_DOWN:
                #    yOffset = (yOffset + 1) % WORLD_Y
                elif event.key == pygame.K_LEFT:
                    xOffset = (xOffset + 1) % WORLD_X
                elif event.key == pygame.K_RIGHT:
                    xOffset = (xOffset - 1) % WORLD_X

        render(xOffset, yOffset)
        pygame.display.update()
        CHRONO.tick(60)

resizeMap()
generateHeight2()
generateRivers()
generateTypes1()
main()
