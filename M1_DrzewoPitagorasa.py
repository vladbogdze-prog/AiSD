import pygame
import math
import sys

#ustawienie parametrow okna
WIDTH, HEIGHT = 1000, 800
BG_COLOR = (255, 255, 255)    # bialy BG
LINE_COLOR = (0, 0, 0)        # czarne linie
TEXT_COLOR = (0, 0, 0)
LOGO_PATH = "kolor_bez_nazwy_logo_filia_wilno_300.png"

pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Drzewo Pitagorasa - Poprawna Geometria") #nazwa okna
clock = pygame.time.Clock()
font = pygame.font.SysFont("Arial", 18) # szrift czcionki

# status app
running = True
paused = False
current_depth_limit = 0.0
max_depth = 10
angle_deg = 45  # kat wierzcholka trojkata
growth_speed = 0.02 # jak szybko rosnie drzewo

# logo
try:
    logo_img = pygame.image.load(LOGO_PATH).convert_alpha()
    ratio = 150 / logo_img.get_width()
    logo_img = pygame.transform.scale(logo_img, (150, int(logo_img.get_height() * ratio)))
except:
    logo_img = None

def get_triangle_tip(p4, p3, angle_alt):
    """oblicza wierzcholek trojkata nad kwadratem"""
    dx = p3[0] - p4[0]
    dy = p3[1] - p4[1]
    
    # kat schylenia podstawy trojkata
    base_angle = math.atan2(dy, dx)
    # dlugosc podstawy
    c = math.sqrt(dx**2 + dy**2)
    
    # kat alpha (lewy) i dlugosc boku b (lewy przyprostokatny)
    alpha = math.radians(angle_alt)
    b = c * math.cos(alpha)
    
    # koordynaty wierzcholka
    tip_x = p4[0] + b * math.cos(base_angle - alpha)
    tip_y = p4[1] + b * math.sin(base_angle - alpha)
    
    return (tip_x, tip_y)

def draw_pythagoras_step(p1, p2, depth, current_limit):
    """
    p1, p2: punkty podstawy kwadratu
    depth: aktualna glebokosc
    current_limit: limit animacji
    """
    if depth > current_limit or depth > max_depth:
        return

    #1 rysuje square
    dx = p2[0] - p1[0]
    dy = p2[1] - p1[1]
    
    #Punkty gorne wierzcholka kwadratu obrot o 90 stopni
    p4 = (p1[0] + dy, p1[1] - dx)
    p3 = (p2[0] + dy, p2[1] - dx)
    
    pygame.draw.polygon(screen, LINE_COLOR, [p1, p2, p3, p4], 1)

    #2 Rysujemy Triangle jesli nie przekraczamy limitu o 0.5 kroku
    #plynna animacji 2d
    if current_limit > depth:
        p_tip = get_triangle_tip(p4, p3, angle_deg)
        pygame.draw.polygon(screen, LINE_COLOR, [p4, p3, p_tip], 1)

        #3 REKURENCJA dla mniejszych kwadratow (galezi Pitagorasa)
        #lewy kwadrat rosnie na boku (p4, p_tip)
        draw_pythagoras_step(p4, p_tip, depth + 1, current_limit)
        # Prawy kwadrat rosnie na boku p_tip, p3
        draw_pythagoras_step(p_tip, p3, depth + 1, current_limit)

def draw_ui():
    if logo_img:
        screen.blit(logo_img, (WIDTH - logo_img.get_width() - 20, 20))
    
    instr = [
        f"Głębokość: {current_depth_limit:.1f} / {max_depth}",
        f"Kąt trójkąta: {angle_deg}° (Strzałki Lewo/Prawo)",
        "P: Pauza | R: Restart | Spacja: Reset",
        "ESC: Wyjście",
        "Zalecenie: Kąt 45° to trójkąt prostokątny równoramienny"
    ]
    for i, text in enumerate(instr):
        img = font.render(text, True, TEXT_COLOR)
        screen.blit(img, (20, 20 + i * 25))

# GUI petla
while running:
    screen.fill(BG_COLOR) #czysci ekran

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_p: paused = not paused
            if event.key == pygame.K_r: current_depth_limit = 0.0
            if event.key == pygame.K_SPACE: current_depth_limit = 1.0
            if event.key == pygame.K_RIGHT: angle_deg = min(80, angle_deg + 2)
            if event.key == pygame.K_LEFT: angle_deg = max(10, angle_deg - 2)
            if event.key == pygame.K_ESCAPE: running = False

    if not paused and current_depth_limit < max_depth:
        current_depth_limit += growth_speed

    #parametry startowego pnia
    size = 120
    p1 = (WIDTH // 2 - size // 2, HEIGHT - 100)
    p2 = (p1[0] + size, p1[1])
    
    # rysowanie od glebokosci 0 (pnia), by na glebokosci 0 byl tylko punkt startowy
    draw_pythagoras_step(p1, p2, 0, current_depth_limit)

    draw_ui()
    pygame.display.flip()
    clock.tick(60)

pygame.quit()