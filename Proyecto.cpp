#include <iostream>
#include <cmath>
#include <fstream>
#include <chrono>

using namespace std;

string argv[4] = {
    "NameProgram",
    "Envio.in",
    "Acelerador.in",
    "Planetas.in"
};

///Estructura de los puntos de aceleración.
struct Point {
    int x = 0, y = 0;

    Point(){};
    Point(int x, int y){
        this->x = x;
        this->y = y;
    }
};

///Estructura de tipo Planeta.
struct Planeta {
    string name;
    float x, y, r;
    
    Planeta(){};
    Planeta(string name, float x, float y, float r){
        this->name = name;
        this->x = x;
        this->y = y;
        this->r = r;
    }
};

//*VARIABLES GLOBALES.
//*--------------------------------------------------------

//---------------------------------------------------------
int rangMin, rangMax, lanzX, lanzY, entregaX, entregaY;
//---------------------------------------------------------

//---------------------------------------------------------
int *solCoeActual, *solCoeMejor, sizeCoeficientes;
int minLaunch = -1;
int energiaAcum = 0;
//---------------------------------------------------------

//---------------------------------------------------------
float maxEnergy;
Point *points;
int sizePoints = 0;
Planeta *planetasArr;
int sizePlanetas = 0;
//---------------------------------------------------------

//*--------------------------------------------------------

//*Función de búsqueda de los coeficientes.
//*--------------------------------------------------------
void searchCoeficientes(int it);
//*--------------------------------------------------------

//*Función que verifica si el arreglo de coeficientes llego al máximo.
//*--------------------------------------------------------
bool maxCoe();
//*--------------------------------------------------------

//*Función que verifica si la suma del valor absoluto de los coeficientes exceden el máximo de energía
//*--------------------------------------------------------
bool excedeMax(int max);
//*--------------------------------------------------------

//*Función que verifica si el polinomio pasa por todo los puntos.
//*--------------------------------------------------------
bool verifyPoints();
//*--------------------------------------------------------

//*Función que verifica que el polinomio no choque con ningun planeta.
//*--------------------------------------------------------
bool isPlanetCrash();
//*--------------------------------------------------------

//*Función que verifica que el polinomio obtenido tenga la mínima velocidad de lanzamiento.
//*--------------------------------------------------------
bool isMinLaunch();
//*--------------------------------------------------------

int main(){
    auto inicio = std::chrono::high_resolution_clock::now();
    
    ///Lectura de los archivos de entrada.
    ///-------------------------------------------------------------
    ifstream envio(argv[1]), acelerador(argv[2]), planetas(argv[3]);
    
    if(envio.is_open() && acelerador.is_open() && planetas.is_open()){

        //*Lectura del archivo de envio.
        //*------------------------------------------------------------
        envio >> rangMin >> rangMax >> maxEnergy >> lanzX >> lanzY >> entregaX >> entregaY;
        //*------------------------------------------------------------
        
        //*Lectura del archivo de Acelerador.
        //*------------------------------------------------------------
        //Primera vuelta para ver cuantos puntos de aceleración tiene el archivo
        string line = "";
        while(getline(acelerador, line)){
            sizePoints++;
        }

        sizePoints += 2;
        points = new Point[sizePoints];

        //Segunda vuelta para obtener cada punto de aceleración y almacenarlo en el arrgelo.
        int itAcln = 0;
        acelerador.clear();
        acelerador.seekg(0, std::ios::beg);
        while(!acelerador.eof()){
            int x, y;
            acelerador >> x >> y;
            Point aceleratorPoint(x,y);
            points[itAcln] = aceleratorPoint;
            itAcln++;
        }
        //*------------------------------------------------------------
        
        //*Lectura del archivo Planetas.
        //*------------------------------------------------------------

        //Primera vuelta para contar cuantos planetas hay en el archivo.
        int countSkipt = 0;
        while(getline(planetas, line)){
            if(countSkipt == 1){
                countSkipt = 0;
                continue;
            }
            sizePlanetas++;
            countSkipt++;
        }

        planetasArr = new Planeta[sizePlanetas];

        ///Segunda vuelta para almacenar los planetas en el arreglo.
        planetas.clear();
        planetas.seekg(0, std::ios::beg);
        int itPlanetas = 0;
        string name; double x,y,r;
        while(getline(planetas, name)){

            planetas >> x >> y >> r;
            Planeta planeta(name,x,y,r);
            planetasArr[itPlanetas] = planeta;

            itPlanetas++;
            planetas >> std::ws;
        }
        //*------------------------------------------------------------

        envio.close();
        acelerador.close();
        planetas.close();
    }else {
        cout << "No se logro abrir los archivos de entrada." << endl;
        return 0;
    }
    ///-------------------------------------------------------------

    //Añado los puntos de lanzamiento y entrega en el arreglo de puntos (points).
    ///-------------------------------------------------------------
    Point puntoLanzamiento(lanzX, lanzY), puntoEntrega(entregaX, entregaY);
    points[sizePoints - 2] = puntoLanzamiento;
    points[sizePoints - 1] = puntoEntrega;
    ///-------------------------------------------------------------

    sizeCoeficientes = sizePoints+1;
    solCoeActual = new int[sizeCoeficientes];
    solCoeMejor = new int[sizeCoeficientes];

    for(int i = 0; i < sizeCoeficientes; i++){
        solCoeActual[i] = 0;
        solCoeMejor[i]= 0;
    }
 
    searchCoeficientes(0);

    int count = 0;
    for(int i = 0; i < sizeCoeficientes; i++){
        if(solCoeMejor[i] == 0) count++;
    }

    if(count == sizeCoeficientes){
        cout << "ERR-11!_NOTFOU";
    }else{
        for(int i = 0; i < sizeCoeficientes; i++){
            if(solCoeMejor[i] == 0) continue;
    
            int exponente = (sizeCoeficientes-1-i);
    
            if(i != 0) cout << " ";
    
            if(solCoeMejor[i] < 0) cout << "-";
            else if(i != 0) cout << "+";
    
            if(i != 0) cout << " ";
    
            if(abs(solCoeMejor[i]) != 1 || i == sizeCoeficientes - 1){
                cout << abs(solCoeMejor[i]);
            }
    
            if(exponente != 0) cout << "x";
            if(exponente > 1) cout << "^" << exponente;
        }
    }


    //*Eliminación de los arreglos dinámicos.
    //*-------------------------------------------------------------
    delete[] points;
    delete[] planetasArr;
    delete[] solCoeActual;
    delete[] solCoeMejor;
    //*-------------------------------------------------------------

    auto fin = chrono::high_resolution_clock::now();
    
    // Calculamos el tiempo transcurrido en microsegundos
    auto duracion = chrono::duration_cast<chrono::microseconds>(fin - inicio);
    
    cout << endl << "Tiempo de ejecucion: " << (duracion.count() / 1000000) << " segundos" << endl;

    return 0;
}

// Definición de la función de la búsqueda de los coeficinetes.
void searchCoeficientes(int it){

    for(int i = rangMin; i <= rangMax; i++){
        energiaAcum += abs(i);
        
        if(energiaAcum > maxEnergy) {
            energiaAcum -= abs(i);
            continue;
        }
        
        solCoeActual[it] = i;
        
        if(it < sizeCoeficientes-1){
            searchCoeficientes(it+1);
        }else{
            if(verifyPoints() && !isPlanetCrash() && isMinLaunch()){
                for(int j = 0; j < sizeCoeficientes; j++){
                    // cout << solCoeActual[j] << " ";
                    solCoeMejor[j] = solCoeActual[j];
                }
            }
        }
        
        energiaAcum -= abs(i);
    }
}

//Definición de la función que verifica si todos los coeficientes de un arreglo llegaron al rango máximo.
bool maxCoe(){
    int count = 0;
        for(int i = 0; i < sizeCoeficientes; i++){
        if(solCoeActual[i] == rangMax){
            count++;
        }
    }
    if(count == sizeCoeficientes){
        return true;
    }
    return false;
}

bool excedeMax(int max){
    int valor = abs(solCoeActual[0]);
    //Verifica que la suma de los coeficientes no excedan el máximo de energía.
    for(int i = 1; i <= max; i++){
        valor += abs(solCoeActual[i]);
    }

    if(valor > maxEnergy){
        return true;
    }

    return false;
}

//Definición de la función que verifica si el polinomio pasa por todos los puntos.
bool verifyPoints(){
    int valor = 0;

    for(int i = 0; i < sizePoints; i++){
        for(int j = 0; j < sizeCoeficientes; j++){
            valor += solCoeActual[j] * pow(points[i].x, (sizeCoeficientes-1) - j);
        }
        if(valor != points[i].y){
            return false;
        }
        valor = 0;
    }

    
    return true;
}

//Definición de la función que verifica que un polinomio no choque con los planetas dados.
bool isPlanetCrash(){
    for(int i = 0; i < sizePlanetas; i++){
        float c = planetasArr[i].x, d = planetasArr[i].y, r = planetasArr[i].r;

        for(float x = c-r; x <= c+r; x += 0.05){
            float y = 0;

            for(int k = 0; k < sizeCoeficientes; k++){
                y += solCoeActual[k] * pow(x, (sizeCoeficientes-1) - k);
            }

            //Se evalua que el y evaluado en el punto x esté dentro del rango de la circunferencia.
            if(pow(x-c, 2) + pow(y-d, 2) <= pow(r,2)){
                return true;
            }
        }
    }

    return false;
}

///Definición de la función que verifica si el polinomio tiene la mínima velocidad de lanzamiento posible.
bool isMinLaunch(){
    int valor = 0;

    for(int i = 0, j = sizeCoeficientes - 1; i < sizeCoeficientes - 1; i++, j--){
        valor += solCoeActual[i] * j * pow(points[sizePoints - 2].x, j-1);
    }

    valor = abs(valor);

    if(minLaunch == -1 || valor < minLaunch){
        minLaunch = valor;
        return true;
    }

    return false;
}
