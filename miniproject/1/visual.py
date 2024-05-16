import numpy as np
import pygame

pygame.init()

screen = pygame.display.set_mode([2000, 2000])
screen.fill((255, 255, 255))
clock = pygame.time.Clock()
FPS = 20

for i in range(1000):
    i*= 10;
    screen.fill((255, 255, 255))

    with open(str(i)+".csv", "r") as f:
        line0 = f.readline()
        x_1, y_1, flag = (np.asarray(list(map(float, line0.split(";"))))*10 + 100)*10
        next(f)
        for line in f:
            x_2, y_2, new_flag = (np.asarray(list(map(float, line.split(";"))))*10 + 100)*10

            dist = np.sqrt((x_1 - x_2)**2 + (y_1 - y_2)**2) # небольшой костыль
            if (new_flag == flag) and (dist < 10):
                pygame.draw.line(screen, (255, 0, 0), [int(x_1), int(y_1)],
                                 [int(x_2), int(y_2)])
            x_1, y_1 = x_2, y_2
            flag = new_flag

    pygame.display.update()
    clock.tick(FPS)
pygame.quit()


