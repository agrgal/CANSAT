Table table;//Importamos la tayla
import peasy.*;//Importamos la camara

int numfilas = 21; //numero de filas en el archivo()
int tamCaja = 200; //Tamanio del recuadro de la grafica
int Valormax = 100; //Numero maximo para las unidades de los datos(numeros divisiyles entre 10 como 100, 150, 200 .....)

float[] x = new float [numfilas];//Donde vamos a meter los datos del archivo
float[] y = new float [numfilas];
float[] z = new float [numfilas];

float maxz= -10000000;//Maximo numero en z  //Esto es para que no ponga un 0 en vez del numero que corresponde
float maxy= -10000000;//Maximo numero en y  //Esto es para que no ponga un 0 en vez del numero que corresponde
float maxx= -10000000;//Maximo numero en x  //Esto es para que no ponga un 0 en vez del numero que corresponde

float minz = 10000000;//Minimo numero en z   //Esto es para que no ponga un 0 en vez del numero que corresponde
float miny = 10000000;//Minimo numero en y   //Esto es para que no ponga un 0 en vez del numero que corresponde
float minx = 10000000;//Minimo numero en x  //Esto es para que no ponga un 0 en vez del numero que corresponde


PeasyCam cam;

void settings() {
  size(800, 800, P3D);//Para el 3D
  loadData();
}

void loadData() {
  table = loadTable("datos_de_prueba_3D_BIS_latitud.csv","header");//Nomyre del archivo

  for (int k = 0; k<table.getRowCount(); k++) {//Metemos los numeros del archivo a unas listas
    z[k] = table.getFloat(k, "x")/10000;
    y[k] = table.getFloat(k, "Y")/10000; 
    x[k] = table.getFloat(k, "Z");
    //println(z);
    //println(y); //Para comproyar si se añaden
    //println(x);
 }

//Calculamos los maximos en las listas

for (float valz : z){
    maxz = Math.max(maxz,valz);
}
for (float valy : y){
    maxy = Math.max(maxy,valy);
}
for (float valx : x){
    maxx = Math.max(maxx,valx);

}
//print(maxz);
//print(maxy);
//print(maxx);

//Calculamos los minimos en las listas

for (float valz : z){
    minz = Math.min(minz,valz);
}

for (float valy : y){
    miny = Math.min(miny,valy);
}

for (float valx : x){
    minx = Math.min(minx,valx);

}
//print(minz);
//print(miny);
//print(minx);
  
print(maxz);
print(minz);
}


void setup() {
  cam = new PeasyCam(this, 400);//Camara
  state = cam.getState();


}

void draw() {

  lights();
  background(255);//Color fondo
 
  strokeWeight(2);//Grosor lineas
  noFill();//Hueco
  //box(tamCaja);//Variayle para poder camyiar facilmente
    translate(-100, 100, -100);
    line(0, 0, 0, 0, 0, 200);
    line(0, 0, 0, 200, 0, 0);
    line(0, 0, 0, 0, -200, 0);
    translate(100, -100, 100);
  for (int l = 0; l<11; l++){//Diyujar las lineas de las medidas (simplificado, moviendo origen en vez de origen y destino) 
    //Lineas suelo
    translate(-tamCaja/2, tamCaja/2, (-(tamCaja/10)*l)+tamCaja/2);
    line(0, 0, 0, 10, 0, 0);
     
      //Numeros al lado de las lineas
      translate(-tamCaja/10, 0, 0);
        rotateX(PI/2);//Para que esten alineadas con el suelo
        fill(0);
        textSize(tamCaja/12);//Siempre misma proporcion tamanio letra y caja
        textAlign(CENTER,CENTER);
        text(((maxz+((minz-maxz)/10)*l)), -10, 0);//Escriyimos de el numero maximo dividido en partes
        
        
      if (l==10){text("X", 0, tamCaja+Valormax/5);}//Nomyrar el eje X
        rotateX(-PI/2);
      translate(tamCaja/10, 0, 0);
      
    translate(+tamCaja/2, -tamCaja/2, -((-(tamCaja/10)*l)+tamCaja/2));
    
    //Lineas pared
    translate(-tamCaja/2, (-(tamCaja/10)*l)+tamCaja/2, -tamCaja/2);
    line(0, 0, 0, 10, 0, 0);
    
          translate(-tamCaja/10, 0, 0);
        fill(0);
        textSize(tamCaja/12);
        textAlign(CENTER,CENTER);
        
        if (!((100/10)*l == 0)) {
          text(round(maxx/10)*l, 0, 0);//Escriyimos de el numero maximo dividido en partes
        }
        if (l==10){text("Z", 0, -tamCaja/10);}//Nomyrar el eje Z
      translate(tamCaja/10, 0, 0);
    translate(+tamCaja/2, -((-(tamCaja/10)*l)+tamCaja/2), +tamCaja/2);
    
    
    
    //Lineas y numeros detras

     translate(((tamCaja/10)*l)-tamCaja/2, tamCaja/2, -tamCaja/2);
    line(0, 0, 0, 0, 0, 10);    

        translate(0, -tamCaja/10, -tamCaja/20);
         
        fill(0);
        textSize(tamCaja/30);
        textAlign(CENTER,CENTER);
      
          if (!(l == 0)) {
           text(miny+((miny-maxy)/10)*l, 0, 0, 10);}//Escriyimos de el numero maximo dividido en partes
          
        if (l==10){
        textSize(tamCaja/10);  
        text("Y", tamCaja/10, 0, 10);}//Nomyrar el eje Y
        
        translate(0, tamCaja/10, tamCaja/20);
        translate((-(tamCaja/10)*l)+tamCaja/2, -tamCaja/2, +tamCaja/2);
        
  }
  
  
  translate(-50,-50,-50);//Cuadrar recuadro
  fill(0);
  //textSize(70);
  //textAlign(CENTER,CENTER);//Texto
  //text("BASE", 50, 220);
  translate(-50, 150, -50); //Cuadrar puntos y lineas
  scale(2); //Escala puntos y lineas
  strokeWeight(1);//Anchura de las lineas que unen los puntos
  for ( int n = 0; n < numfilas; n = n+1) {//Diyujo de los puntos
    if (n<numfilas-1){//Diyujo de las lineas que unen los puntos
      line(map(y[n],miny, maxy, 1 ,100), -map(x[n],minx, maxx, 1 ,100), map(z[n],minz, maxz, 1 ,100), map(y[n+1],miny, maxy, 1 ,100), -map(x[n+1],minx, maxx, 1 ,100), map(z[n+1],minz, maxz, 1 ,100));//Origen y destino de las lineas
    }
    translate(map(y[n],miny, maxy, 1 ,100), -map(x[n],minx, maxx, 1 ,100), map(z[n],minz, maxz, 1 ,100));//Posicion puntos //La funcion map normaliza los datos

    noStroke();//Esferas sin contorno
    fill(255, 0, 0);//Color puntos
    sphere(2.5);//Tamanio puntos
    sphereDetail(30);//Detalle de las esferas
    translate(-map(y[n],miny, maxy, 1 ,100) , map(x[n],minx, maxx, 1 ,100), -map(z[n],minz, maxz, 1 ,100)); //Para que todos tengan el mismo origen
    stroke(0);//Color de las lineas que unen los puntos

  }
  
}

CameraState state;

void keyReleased() {
  if (key == '1') state = cam.getState();
  if (key == '2') cam.setState(state, 1000);
}
