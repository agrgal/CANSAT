Table table;//Importamos la tayla
import peasy.*;//Importamos la camara
import java.util.Arrays; //Para no pegarme un tiro :)
PFont myFont;/////////Fuente con mejor resulucion
Table temp_rot_pres;////////Tabla
PImage fondosuelo;////Imagenes
PImage fondocielo;
PImage fondo;
PImage fondointerfaz;


float w = 1.5;////////Tamanio de los "puntos" de las graficas
float h = 4;
float d = 1.5;

int tamCaja = 200; //Tamanio del recuadro de la grafica
int Valormax = 100; //Numero maximo para las unidades de los datos(numeros divisiyles entre 10 como 100, 150, 200 .....)

int numfilas = 0; //numero de filas en el archivo()

//Donde vamos a meter los datos del archivo

ArrayList<Float> x = new ArrayList<Float>();
ArrayList<Float> y = new ArrayList<Float>();
ArrayList<Float> z = new ArrayList<Float>();

ArrayList<Float> rotX = new ArrayList<Float>();
ArrayList<Float> rotY = new ArrayList<Float>();
ArrayList<Float> temp = new ArrayList<Float>();
ArrayList<Float> presion = new ArrayList<Float>();



float maxz= -10000000;//Maximo numero en z  //Esto es para que no ponga un 0 en vez del numero que corresponde
float maxy= -10000000;//Maximo numero en y  //Esto es para que no ponga un 0 en vez del numero que corresponde
float maxx= -10000000;//Maximo numero en x  //Esto es para que no ponga un 0 en vez del numero que corresponde

float minz = 10000000;//Minimo numero en z   //Esto es para que no ponga un 0 en vez del numero que corresponde
float miny = 10000000;//Minimo numero en y   //Esto es para que no ponga un 0 en vez del numero que corresponde
float minx = 0;//Minimo numero en x  //Esto es para que no ponga un 0 en vez del numero que corresponde

float mintemp = 100000000;////Mismo arriba
float maxtemp = -10000000;

float minpresion = 1000000000;///////Mismo arriba arriba
float maxpresion = -1000000000;

PeasyCam cam;//////Para que funcione la camara

void settings() {
  size(1200, 1000, P3D);//Para el 3D
  loadData();

}

void loadData() {

  
  table = loadTable("data (1).csv","header");//Nomyre del archivo

  for (int k = 0; k<table.getRowCount(); k++) {//Metemos los numeros del archivo a unas listas

    if (((table.getFloat(k, "Presion") != 0 ) ||  (table.getFloat(k, "Temperatura") != 0 ) || (table.getFloat(k, "Ángulo en y") != 0 ) || (table.getFloat(k, "Ángulo en x") != 0 ) || (table.getFloat(k, "Longitud") != 0) || (table.getFloat(k, "Latitud") != 0) || (table.getFloat(k, "Altura") != 0)) && (table.getFloat(k, "Temperatura") < 100) ) {  
  x.add(table.getFloat(k, "Altura"));
  z.add(table.getFloat(k, "Latitud"));
  y.add(table.getFloat(k, "Longitud"));
  rotY.add(table.getFloat(k, "Ángulo en x"));
  rotX.add(table.getFloat(k, "Ángulo en y"));
  temp.add(table.getFloat(k, "Temperatura"));
  presion.add(table.getFloat(k, "Presion"));
  numfilas = numfilas + 1;
  }
    //println(z);
    //println(y); //Para comproyar si se añaden
    //println(x);
 }
println(temp);
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

//for (float valx : x){
  //  minx = Math.min(minx,valx);

//}
///////////////////Pruebas
//print(minz);
//print(miny);
//print(minx);
  
//print(maxz);
//print(minz);

///////////////////////////////////////////Datos de temperatura, rotacion y presion
 // temp_rot_pres = loadTable("data pruebas reales 1.csv","header");//Nomyre del archivo

 // for (int k = 0; k<temp_rot_pres.getRowCount(); k++) {//Metemos los numeros del archivo a unas listas
 // if ((temp_rot_pres.getFloat(k, "Presion") != 0 ) || (temp_rot_pres.getFloat(k, "Temperatura") != 0 ) || (temp_rot_pres.getFloat(k, "Ángulo en y") != 0 ) || (temp_rot_pres.getFloat(k, "Ángulo en x") != 0 )) {  
 // rotY.add(temp_rot_pres.getFloat(k, "Ángulo en x"));
//  rotX.add(temp_rot_pres.getFloat(k, "Ángulo en y"));
//  temp.add(temp_rot_pres.getFloat(k, "Temperatura"));
//  presion.add(temp_rot_pres.getFloat(k, "Presion"));
//  numfilas = numfilas + 1;
//  }
//}
///////////////////Valores maximos y minimos 
for (float valtemp : temp){
    mintemp = Math.min(mintemp,valtemp);
}

for (float valtemp : temp){
    maxtemp = Math.max(maxtemp,valtemp);
}
//println(maxtemp);
for (float valpresion : presion){
    minpresion = Math.min(minpresion,valpresion);
}

for (float valpresion : presion){
    maxpresion = Math.max(maxpresion,valpresion);
}
/////////////////////////////////////////////////////////////////////////////////////////
}
//////////////////////////Poner camara y todas las imagenes y reescalandolas, y la fuente de texto
void setup() {
  cam = new PeasyCam(this, 600);//Camara
  state = cam.getState();
  fondo = loadImage("brawh.png");
  fondosuelo = loadImage("fondosuelo.jpg");
  fondosuelo.resize(200,200);
  fondointerfaz = loadImage("fondointerfaz.jpeg");
  fondointerfaz.resize(250,0);
  fondocielo = loadImage("fondocielo.jpg");
  fondocielo.resize(200,200);
  myFont = createFont("Arial", 30);

}
///////////////////////////////////////////////////////////////////////////////
void draw() {
  float[] rotations = cam.getRotations();
  float yaw = rotations[1];
  float pitch = rotations[0];
  
  background(255);//Color fondo
  
  //lights();

/////////////////////////////////Ejes,lineas y numeros de los datos/////////////////////

  strokeWeight(2);//Grosor lineas
  noFill();//Hueco
  //box(tamCaja);//Variayle para poder camyiar facilmente
    translate(-250, 100, -100);
      rotateX(PI/2);
      image(fondosuelo,0,0);//Imagen de tierra
      rotateX(-PI/2);
      translate(0,0,-0.5); // Para que no se superponga con los numeros
      image(fondocielo,0,-200);//Imagen cielo
      translate(0,0,0.5);
      
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
        
        
      if (l==10){text("Latitud", 0, tamCaja+Valormax/5);}//Nomyrar el eje X
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
        
        if (!((10)*l == 0)) {
          text((maxx/10)*l, -10, 0);//Escriyimos de el numero maximo dividido en partes
        }
        if (l==10){text("Altura", 0, -tamCaja/10);}//Nomyrar el eje Z
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
           text(miny-((miny+maxy)/10)*l, 0, 0, 10);}//Escriyimos de el numero maximo dividido en partes
          
        if (l==10){
        textSize(tamCaja/10);  
        text("Longitud", tamCaja/10, 0, 10);}//Nomyrar el eje Y
        
        translate(0, tamCaja/10, tamCaja/20);
        translate((-(tamCaja/10)*l)+tamCaja/2, -tamCaja/2, +tamCaja/2);
        
  }
  


  translate(-50,-50,-50);//Cuadrar recuadro
  fill(0);
  textFont(myFont, 30);
  textSize(30);
  textAlign(CENTER,CENTER);//Texto
  text("Posicion", 50, 220);
  translate(-50, 150, -50); //Cuadrar puntos y lineas
  
  scale(2); //Escala puntos y lineas
  strokeWeight(1);//Anchura de las lineas que unen los puntos
 //////////////////////////////////////////////////////////////////////////////////////////////////////
 
///////////////////////////////Dibujo de puntos y lineas //////////////////////////////////////  
  //println(numfilas);
  for ( int n = 0; n < numfilas; n = n+1) {                    //Diyujo de los puntos
    if (n<numfilas-1){//Diyujo de las lineas que unen los puntos
      line(map(y.get(n),miny, maxy, 1 ,100), -map(x.get(n),minx, maxx, 1 ,100), map(z.get(n),minz, maxz, 1 ,100), map(y.get(n+1),miny, maxy, 1 ,100), -map(x.get(n+1),minx, maxx, 1 ,100), map(z.get(n+1),minz, maxz, 1 ,100));//Origen y destino de las lineas
    }
    translate(map(y.get(n),miny, maxy, 1 ,100), -map(x.get(n),minx, maxx, 1 ,100), map(z.get(n),minz, maxz, 1 ,100));//Posicion puntos //La funcion map normaliza los datos

    noStroke();//Esferas sin contorno
    fill(#9C40B7);//Color puntos
    //box(2.5);//Tamanio puntos
    
    rotateX(rotX.get(n)*PI/180);//
    rotateY(rotY.get(n)*PI/180);
  beginShape(QUADS);/////////////Forma de los "puntos"
  vertex(-w/2, -h/2, d/2);
  vertex(w/2, -h/2, d/2);
  vertex(w/2, h/2, d/2);
  vertex(-w/2, h/2, d/2);
  
  vertex(-w/2, -h/2, -d/2);
  vertex(w/2, -h/2, -d/2);
  vertex(w/2, h/2, -d/2);
  vertex(-w/2, h/2, -d/2);
  endShape();
  
  beginShape(QUAD_STRIP);
  vertex(-w/2, -h/2, d/2);
  vertex(-w/2, -h/2, -d/2);
  vertex(w/2, -h/2, d/2);
  vertex(w/2, -h/2, -d/2);
  vertex(w/2, h/2, d/2);
  vertex(w/2, h/2, -d/2);
  vertex(-w/2, h/2, d/2);
  vertex(-w/2, h/2, -d/2);
  vertex(-w/2, -h/2, d/2);
  vertex(-w/2, -h/2, -d/2);
  endShape();
        
        rotateY(-rotY.get(n)*PI/180);
        rotateX(-rotX.get(n)*PI/180);

    sphereDetail(30);//Detalle de las esferas
    translate(-map(y.get(n),miny, maxy, 1 ,100) , map(x.get(n),minx, maxx, 1 ,100), -map(z.get(n),minz, maxz, 1 ,100)); //Para que todos tengan el mismo origen
    stroke(0);//Color de las lineas que unen los puntos

  }
  //////////////////////////////////////////////////////////////////////////
  
  //////////////Dibujo de ejes temperatura///////////////////////
  scale(0.5);
  strokeWeight(2);//Grosor lineas
  noFill();//Hueco
  //box(tamCaja);//Variayle para poder camyiar facilmente
    translate(350, 0, 0);
    line(0, 0, 0, 200, 0, 0);
    line(0, 0, 0, 0, -200, 0);
    translate(100, -100, 100);
  for (int l = 0; l<11; l++){//Diyujar las lineas de las medidas (simplificado, moviendo origen en vez de origen y destino)   
    //Lineas pared
    translate(-tamCaja/2, (-(tamCaja/10)*l)+tamCaja/2, -tamCaja/2);
    line(0, 0, 0, 10, 0, 0);
    
        translate(-tamCaja/10-10, 0, 0);
        fill(0);
        textSize(tamCaja/12);
        textAlign(CENTER,CENTER);
        
        if (!((100/10)*l == 0)) {
          text(round(minpresion + (maxpresion - minpresion)/10*l), 0, 0);//Escriyimos de el numero maximo dividido en partes
      //println(minpresion);  
      //println(maxpresion);  
      }
        if (l==10){text("Presion", 0, -tamCaja/10);}//Nomyrar el eje "Z" la presion
      translate(tamCaja/10+10, 0, 0);
    translate(+tamCaja/2, -((-(tamCaja/10)*l)+tamCaja/2), +tamCaja/2);
    
    //Lineas y numeros detras

     translate(((tamCaja/10)*l)-tamCaja/2, tamCaja/2, -tamCaja/2);
    line(0, 0, 0, 0, 0, 10);    

        translate(0, -tamCaja/10, -tamCaja/20);
         
        fill(0);
        textSize(tamCaja/24);
        textAlign(CENTER,CENTER);
      
          if (!(l == 0)) {
           text((nfc((mintemp +(maxtemp - mintemp)/10*l), 1)), 0, 0, 10);}//Escriyimos de el numero maximo dividido en partes

        if (l==10){
        textSize(tamCaja/10);  
        text("Temperatura", tamCaja/10 + 70, 10, 0);}//Nomyrar el eje Y
        
        translate(0, tamCaja/10, tamCaja/20);
        translate((-(tamCaja/10)*l)+tamCaja/2, -tamCaja/2, +tamCaja/2);
  }

  translate(-50,-50,-50);
  translate(0, 150, -50);

  scale(2);
/////////////////////////////////////////////////////////////////
  
  ///////////////////////////Dibujo de puntos temp y presion//////////////////////////////////
  
  
  strokeWeight(1);//Anchura de las lineas que unen los puntos
  for ( int n = 0; n < numfilas; n = n+1) {                    //Diyujo de los puntos
    if (n<numfilas-1){//Diyujo de las lineas que unen los puntos
      line(map(presion.get(n),minpresion, maxpresion, 1 ,100), -map(temp.get(n),mintemp, maxtemp, 1 ,100), 0,  map(presion.get(n+1),minpresion, maxpresion, 1 ,100), -map(temp.get(n+1),mintemp, maxtemp, 1 ,100),0);//Origen y destino de las lineas
    }
    translate(map(presion.get(n),minpresion, maxpresion, 1 ,100), -map(temp.get(n),mintemp, maxtemp, 1 ,100), 0);//Posicion puntos //La funcion map normaliza los datos

    fill(#9C40B7);//Color puntos
  noStroke();
  beginShape(QUADS);
  vertex(-w/2, -h/2, d/2);
  vertex(w/2, -h/2, d/2);
  vertex(w/2, h/2, d/2);
  vertex(-w/2, h/2, d/2);
  
  vertex(-w/2, -h/2, -d/2);
  vertex(w/2, -h/2, -d/2);
  vertex(w/2, h/2, -d/2);
  vertex(-w/2, h/2, -d/2);
  endShape();
  
  beginShape(QUAD_STRIP);
  vertex(-w/2, -h/2, d/2);
  vertex(-w/2, -h/2, -d/2);
  vertex(w/2, -h/2, d/2);
  vertex(w/2, -h/2, -d/2);
  vertex(w/2, h/2, d/2);
  vertex(w/2, h/2, -d/2);
  vertex(-w/2, h/2, d/2);
  vertex(-w/2, h/2, -d/2);
  vertex(-w/2, -h/2, d/2);
  vertex(-w/2, -h/2, -d/2);
  endShape();
  stroke(0);
    sphereDetail(30);//Detalle de las esferas
    translate(-map(presion.get(n),minpresion, maxpresion, 1 ,100) , map(temp.get(n),mintemp, maxtemp, 1 ,100), 0);
  }
//Cuadrar recuadro
  
  scale(0.5);
  fill(0);
  textSize(30);
  textAlign(CENTER,CENTER);//Texto
  text("Temp y Presion", 50, 70, 50);
    ///////////////////////////////////////////////////

///////////////////////////////////////Interfaz/////////////////////////////////////////
  cam.beginHUD();
  pushMatrix();
  translate(0,0,0);
  image(fondointerfaz,0,0);
  popMatrix();
  cam.endHUD();
}
///////////////////////////////////////////////////////////////////////////////////////////
CameraState state;

void keyReleased() {
  if (key == '1') state = cam.getState();
  if (key == '2') cam.setState(state, 1000);
}
