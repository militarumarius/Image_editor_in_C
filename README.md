### Copyright Militaru-Ionut-Marius 

### Editor Foto

### Descriere:

* Programul dat efectueaza diferite operatii cu imagini incarcate dintr-un 
fisier in functie de comanda citita.Citirea se face fie din fisiere binare fie
din fisiere ascii.

* Pentru fiecare comanda  se apeleaza functia corespunzatoare .In caz ca
comanda nu este recunoscuta se afiseaza mesajul : "Invalid command".Iar daca 
nu a fost incarcata nici o poza se afiseaza mesajul : "No image loaded", 
lucru verificat prin load din structura imagine.

### Citirea comenzii o efectuez astfel:

* initial citesc primul cuvant si il compar cu cele din comenzile acceptate
* dupa citesc restul liniei cu o strcutura cuvant_p in care retin propozitia
citita si cuvintele obtinute.
* In functie de cuvintele citite am o functie care returneaza numarul lor 
pentru a verifica daca parametrii sunt valizi sau nu.
* Pentru functiile care necesita drept parametru numere, prin functia 
cifreint transform char in int, iar daca acest lucru nu este posibil 
returnez 0.5 .
* Programul se opreste cand introduc comanda "EXIT"
  
### Comenzile aplicate pe poze sunt urmatoarele:
* "LOAD nume fisier": incarc poza care se afla in fisierul dat, in functie 
de tipul de fisier introdus.Se pot citi 4 tipuri de poze P2,P3,P5,P6 fiecare
avand un tipar.Cand citesc din fisier verific sa sar peste comentarii.
In structura imagine retin coordonatele matricei, respectiv matricea 
corespunzatoare pozei si coordonatele selectiei si tipul imaginei.
Atribui valoarea 1 sau 3 variabilei rgb din structura imagine pentru a sti 
ce fel de poza citesc , color sau gri.In functie de aceasta variabila aloc 
memorie matricei poza, si o voi folosi si pentru celelalte comenzi.
* "SELECT ALL": selecteaza toata poza.
* "SELECT x1 y1 x2 y2": selecteaza pixeli corespunzatori.Daca acestia sunt 
invalizi se afiseaza un mesaj corespunzator.
* "CROP": face crop la selectia curenta daca este o imagine in memorie.La 
finalul acestei operatii schimb indicii corespunzatori liniei, respectiv
coloanei matricei in care retin noua poza.
* "HISTOGRAM X Y": calculeaza histograma pozei gri in functie de numarul de 
binuri citite si de numarul maxim de stelute pentru X.Calculez frecventa
unei grupari adunand frecventa fiecarei valori din grupare.La final afisez 
in functie de rotunzirea obtinuta stelute pentru fiecare bin.
* "EQUALIZE": functioneaza doar pentru poze gri la fel ca HISTOGRAM , si 
modifica valoarea pixelului in functie de suma frecventelor valorilor
anterioare.Retin suma pentru fiecare valoare intr-un vector si dupa fac round 
la raportul specificat, iar apoi clamp pentru a nu iesi din valoarea maxima
a unui pixel.
* "APPLT parametru": aceasta comanda este valabila doar pentru poze rgb si 
modifica poza in functie de parametrul dat.Pentru fiecare tip de apply retin 
o matrice specifica si o valoare la care impart la final.Calculez suma 
corespunzatoare fiecarui pixel care nu se afla pe marginea pozei in functie
de vecinii sai si de matricea specifica si dupa impart suma la valoarea
retinuta. Rezultatul il rotunjesc si dupa ii fac clamp si il atribui 
valorii din matrice.Pentru ca am folosit o singura matrice pentru a calcula 
valoarea vecinului fiecarei culorii am folosit o functie care in functie de 
parametrul cu care o apelezi 0, 1 , 2 returneaza valoarea dorita.
* "ROTATE x": roteste poza selectata la x grade. X trebuie sa fie multiplu de 
90 pentru a putea realiza rotatia. In funntie de unghi rotesc poza de mai 
multe ori la stanga la 90 de grade pentru a realiza rotatia dorita.
Am realizat doua functii care rotesc poza in functie de tipul ei , gri sau rgb 
* "SAVE nume_fisier ascii": salveaza in format ascii la adresa nume_fisier poza
retinuta in acel timp in memorie si dupa totul ramane neschimbat.
* "SAVE nume_fisier": salveaza in format binar la adresa numee_fisier poza 
retinuta in acel timp in memorie si dupa totul ramane neschimbat.
*  "EXIT": cand am aceasta comanda opresc executia programului si eliberez
memorie in care retineam poza.
