README - Tema 1, APD
Andrei Cherechesu, 332CB

				---- TASK 1 ----

Pentru a rezolva Task 1, am definit cateva structuri auxiliare, precum:
	
	- structura pentru memorarea valorilor unui pixel color: 
		typedef struct {
    			unsigned char R, G, B;
		} pixel_t;    
	
	- structura image folosita de functiile predefinite in scheletul temei:
		typedef struct {
			unsigned char pic_type;
			unsigned int width, height;
			unsigned char maxval;
			pixel_t **color_picture;
			unsigned char **grayscale_picture;
		} image;

	- structura pasata threadFunction()-ului, ce contine si limitele
	alocate fiecarui thread, in scopul paralelizarii calculelor:
		typedef struct {
			image *in, *out;  
			unsigned int start, end;
		} thread_data_t;


	Pentru inceput, in functia readInput(), se deschide fisierul de input 
si se citeste header-ul format din pic_type(5 sau 6), width, height si maxval a
pixelilor.
	Apoi, in functie de valoarea citita in pic_type, se aloca spatiu in
unul din campurile color_picture sau grayscale_picture ale variabilei de tip
image primita ca parametru.
	Se citesc cate img->width bytes, de img->height ori, folosind functia
fread(), din fisierul de input, in campul corespunzator alocat mai devreme
(conform cu tipul imaginii - color sau alb-negru). Se inchide, apoi, fisierul
de input.

	La apelul functiei resize(), se aloca spatiu pentru o noua matrice
unde va fi stocata imaginea redimensionata, conform cu noile ei dimensiuni.
Acestea vor fi dimensiunile imaginii originale impartite la resize_factor.
	Se calculeaza intervalul pe care va actiona fiecare thread (mai exact,
ce coloane ii vor fi assignate), si se va memora pentru fiecare thread intr-o
variabila de tip thread_data_t in campurile start si end. Aceasta ii va fi
pasata functiei threadFunction(), la crearea fiecarui thread.
	Pentru obtinerea noii imagini, se face distinctia intre cele 4 
cazuri posibile:
		- alb-negru && resize_factor != 3
		- color && resize_factor != 3
		- alb-negru && resize_factor == 3
		- color && resize_factor == 3

	In situatiile cu resize_factor != 3, fiecare thread face, pentru
fiecare pixel care i-a fost assignat (i = [0, out->height), j = [start, end)),
din noua imagine, suma pixelilor celor resize_factor^2 pixeli din imaginea
originala (adica resize_factor pixeli pe inaltime si resize_factor pixeli pe 
latime, incepand de la index-ul i * resize_factor, respectiv j * resize_factor,
si pana la (i + 1) * resize factor, respectiv (j + 1) * resize_factor,
exclusiv. Se imparte suma obtinuta la resize_factor^2 si se memoreaza
in noua matrice a imaginii pe pozitiile i si j.
	Diferenta dintre cazurile cu imagini alb-negru si color este aceea ca,
la imaginile color, valorile pixelilor vor fi calculate pe fiecare canal in
parte si memorate intr-o matrice de tip pixel_t, iar la cele alb-negru,
valoarea fiecarui pixel va fi tinuta intr-o matrice de tip unsigned char. 
	
	In situatiile cu resize_factor == 3, se procedeaza aproape la fel
ca in cazul anterior, diferenta facand-o faptul ca fiecare pixel din patratul
cu latura resize_factor din imaginea originala este inmultit cu coeficientul
asociat pozitiei sale din Kernelul Gaussian si abia apoi este adaugat la suma.
Dupa ce s-au adaugat toti pixelii la suma, aceasta este impartita la 16 si
memorata ca valoare a pixelului din imaginea noua.

	Dupa terminarea resize()-ului, se scriu header-ul si valorile
din noua matrice obtinuta in fisier-ul de output, in functia writeData().



				---- TASK 2 ----

	Pentru Task 2, se folosesc doar structurile image si thread_data_t, ale
task-ului anterior, insa la structura image a mai ramas doar un singur camp
asociat memorarii valorilor pixelilor, de tip unsigned char, iar la structura
thread_data_t vom avea doar un pointer catre imaginea finala (nemaiexistand
o imagine initiala).

	In functia initialize() se scrie header-ul corespunzator unei imagini
noi alb-negre cu rezolutia resolution primita ca input, si se aloca spatiu
pentru o imagine de resolution * resolution pixeli.

	Functia render() stabileste limitele fiecarui thread (pe ce pixeli
va opera acesta), salveaza valorile necesare in structura thread_data_t
pasata corespunzator fiecarui thread la creare.

	Pentru a putea calcula distanta de la fiecare pixel la dreapta data,
se calculeaza cati pixeli / cm de spatiu logic avem, in variabila scale, 
impartind lungimea/latimea imaginii la lungimea/latimea spatiului logic (100).
Apoi, vom avea nevoie de dimensiunea unei jumatati de latura a fiecarui pixel
(se obtine cu formula 1 / (2 * scale)), in centimetri (adica in spatiul logic).
	
	Pentru fiecare pixel din imagine, se calculeaza coordonatele acestuia,
astfel: x_coord = j / scale + half_pixel si y_coord = i / scale + half_pixel.
Se foloseste formula distantei de la un punct la o dreapta, iar daca distanta
rezultata e mai mica de 3, pixelul va avea culoarea neagra. Altfel, acesta
va fi alb.

	La sfarsit, se scriu header-ul si matricea de pixeli in fisierul
de output.
