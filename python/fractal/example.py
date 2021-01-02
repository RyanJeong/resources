import turtle
import random
# import math   ; unused package

turtle.title("turtle example") #제목
turtle.setup(width=1000, height=800) #창 크기
screen=turtle.Screen()
turtle.colormode(255) #256개 색상펜


base=turtle.Turtle()
base.shape("classic") #거북이 모양
base.setpos(0,0) # 시작지점
base.left(90) #기본 설정에서 turtle이 오른쪽을 향하므로, 다시 위(나무방향)를 향하게
for i in range(40):
    base.forward(10 + 3*i)
    base.left(89.5)
