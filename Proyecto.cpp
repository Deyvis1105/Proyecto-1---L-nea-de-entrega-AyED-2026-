#include <iostream>
#include <cmath>
#include <fstream>

using namespace std;

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
    double x, y, r;
    
    Planeta(){};
    Planeta(string name, double x, double y, double r){
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

//*Función que verifica si al momento de imprimir el polinomio no haya una cadena de coeficientes en 0 desde la posición del indice hasta el indice 0.
//*--------------------------------------------------------
bool existCadena(int it);
//*--------------------------------------------------------

int main(int argc, char* argv[]){
    
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
        //Primera vuelta para ver cuantos puntos de aceleración hay en el archivo.
        string line = "";
        while(getline(acelerador, line)){
            sizePoints++;
        }

        sizePoints += 2;
        points = new Point[sizePoints];

        acelerador.clear();
        acelerador.seekg(0, std::ios::beg);
        
        //Segunda vuelta para almacenar los puntos en un arreglo
        int itAcln = 0, tempX, tempY;
        while(acelerador >> tempX >> tempY){
            points[itAcln] = Point(tempX, tempY);
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

        //Segunda vuelta para almacenar los planetas en el arreglo.
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
    //-------------------------------------------------------------

    //Añado los puntos de lanzamiento y entrega en el arreglo de puntos (points).
    //-------------------------------------------------------------
    Point puntoLanzamiento(lanzX, lanzY), puntoEntrega(entregaX, entregaY);
    points[sizePoints - 2] = puntoLanzamiento;
    points[sizePoints - 1] = puntoEntrega;
    //-------------------------------------------------------------

    //Inicializa los arreglos dinámicos de los Puntos de aceleración y planetas.
    //-------------------------------------------------------------
    sizeCoeficientes = sizePoints+1;
    solCoeActual = new int[sizeCoeficientes];
    solCoeMejor = new int[sizeCoeficientes];
    //-------------------------------------------------------------

    //Inicializa los arreglos de coeficientes "actuales" y los "mejores obtenido" en cero.
    //-------------------------------------------------------------
    for(int i = 0; i < sizeCoeficientes; i++){
        solCoeActual[i] = 0;
        solCoeMejor[i]= 0;
    }
    //-------------------------------------------------------------
 
    searchCoeficientes(0);

    //Verifica si todos los coeficientes están en ceros mediante un contador,
    //lo que se haya obtenido un polinomio y no haya dado un error.
    //-------------------------------------------------------------
    int count = 0;
    for(int i = 0; i < sizeCoeficientes; i++){
        if(solCoeMejor[i] == 0) count++;
    }
    //-------------------------------------------------------------

    if(count == sizeCoeficientes){
        cout << "ERR-11!_NOTFOU";
    }else{
        
        for(int i = 0; i < sizeCoeficientes; i++){
            if(solCoeMejor[i] == 0) continue;
    
            int exponente = (sizeCoeficientes-1-i);

            if(existCadena(i)){
                if(solCoeMejor[i] < 0) cout << "-";
            }else{
                if(i != 0) cout << " ";
                if(solCoeMejor[i] < 0) cout << "-";
                else if(i != 0) cout << "+";
                if(i != 0) cout << " ";
            }
            
    
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
                    solCoeMejor[j] = solCoeActual[j];
                }
            }
        }
        
        energiaAcum -= abs(i);
    }
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
        double c = planetasArr[i].x, d = planetasArr[i].y, r = planetasArr[i].r;

        for(double x = c-r; x <= c+r; x += 0.05){
            
            double y = solCoeActual[0];
            for(int k = 1; k < sizeCoeficientes; k++){
                y += solCoeActual[k] * pow(x, (sizeCoeficientes-1) - k);
            }

            double dx = x-c;
            double dy = y-d;

            //Se evalua que la imagen evaluado en el punto "x" esté dentro del rango de la circunferencia.
            if((dx*dx) + (dy*dy) <= r*r){
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

bool existCadena(int it){
    int count = 0;
    for(int i = it-1; i >= 0; i--){
        if(solCoeMejor[i] == 0) count++;
    }

    if(count == it)
        return true;
    return false;
}
