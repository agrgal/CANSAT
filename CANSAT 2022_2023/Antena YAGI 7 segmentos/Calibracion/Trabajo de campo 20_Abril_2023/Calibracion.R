#Apartado 1
datos= read.csv("RESUMEN.csv", header= TRUE,dec=",")
# datos=datos[1:4,]
datos=head(datos,5)

# Apartado 2
# plot(datos$D, datos$P1, type = "l", main="Antena 1 - Brida blanca")
# plot(datos$D, datos$P2, type = "l", main="Antena 2 - Negra")
# plot(datos$D, datos$P3, type = "l", main="Antena 3 - amarilla")

# regresión lineal 1
# NO --> Suponiendo P = Po exp (-alpha D)
# Suponiendo P = Po + b/ (d^3)

for (i in 2:4) {

x = 1/ (datos$D**3)
y = datos[,i]

l1=lm(y~x)
# plot(x,y,type="l")

po = l1$coefficients[1]
b = l1$coefficients[2]
R2 = summary(l1)$r.squared

f1=function(x) {po+b/(x**3)}

# Apartado 3
plot(datos$D, y, type = "l", main=paste0("Antena ",i-1))
lines(datos$D, f1(datos$D), type = "l",col="red")
print(paste0("P[",i-1,"]=",po,b,"/(d^3)  con R2=",R2))
}
