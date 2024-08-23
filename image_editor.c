// Copyright Militaru-Ionut-Marius

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define lg_max 100

typedef struct {
	char tip[4];
	int lin, col;
	int max_pix;
	int x1, x2, y1, y2;
	int **mat;
	int load;
	int rgb; // pentru pozele rgb = 3 pentru pozele non-rgb = 1;
} imagine;

typedef struct {
	char prop[lg_max];
	char cuv[lg_max][lg_max];

} cuvinte_p;

void declarare(imagine *img)
{
	(*img).load = 0;
	(*img).mat = NULL;
}

// aloc memorie pentru o matrice tinand cont de dimensiunile sale
void alocare_matrice(int lin, int col, int ***mat, int rgb)
{
	int **aux = (int **)malloc(lin * sizeof(int *));
	if (!aux) {
		// daca nu pot aloca, opresc executia
		return;
	}
	for (int i = 0; i < lin; i++) {
		aux[i] = (int *)malloc(rgb * col * sizeof(int));
		// daca una din alocari a esuat, trebuie sa dezaloc tot ce am alocat
		if (!aux[i]) {
			for (int j = 0; j < i; j++)
				free(aux[j]);
			free(aux);
			return;
		}
	}
	*mat = aux;
}

// ignor comentariile din fisiere
void ignorare_com(FILE *input)
{	char carac, a[lg_max];
	fscanf(input, "%c", &carac);
	if (carac == '#')
		fgets(a, lg_max, input);
	else
		fseek(input, -1, SEEK_CUR);
}

// selectez toata poza
void selectietotala(imagine *img)
{
	(*img).x1 = 0;
	(*img).y1 = 0;
	(*img).x2 = (*img).col;
	(*img).y2 = (*img).lin;
}

// fac clamp pentru functiile equalize si apply
int clamp(int x, int min, int max)
{
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}

// pentru k = 0 calculeaza suma vecinilor lui red
// pentru k = 1 calculeaza suma vecinilor lui green
// pentru k = 2 calculeaza suma vecinilor lui blue
// frac reprezinta fractia cu care impart matricea
int suma_vecin(imagine *img, int l, int c, int **mat2, int k, int frac)
{
	if (l == 0 || c == 0 || c == 3 * ((*img).col - 1) || l == ((*img).lin - 1))
		return (*img).mat[l][c + k];
	double s = 0;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			s += (*img).mat[l + i - 1][c + 3 * (j - 1) + k] * mat2[i][j];
	s = round(s / frac);
	s = clamp(s, 0, 255);
	return s;
}

// eliberez o matrice
void free_mat(int **mat, int lin)
{
	for (int i = 0; i < lin; i++)
		free(mat[i]);
	free(mat);
}

// eliberez matricea pozei
void free_image(imagine *img)
{
	for (int i = 0; i < (*img).lin; i++)
		free((*img).mat[i]);
	free((*img).mat);
}

// citesc poza dintr-un fisier ascii
void citire_poza_ascii(imagine *img, FILE *input)
{
	int **aux;
	alocare_matrice((*img).lin, (*img).col, &aux, (*img).rgb);
	(*img).mat = aux;
	for (int i = 0; i < (*img).lin; i++)
		for (int j = 0; j < (*img).rgb * (*img).col; j++)
			fscanf(input, "%d", &(*img).mat[i][j]);
	fclose(input);
	selectietotala(img);
}

// citesc poza dintr-un fisier binar
void citire_poza_binar(imagine *img, FILE *input)
{
	int **aux;
	alocare_matrice((*img).lin, (*img).col, &aux, (*img).rgb);
	(*img).mat = aux;
	for (int i = 0; i < (*img).lin; i++) {
		unsigned char crt;
		for (int j = 0; j < (*img).rgb * (*img).col; j++) {
			fread(&crt, sizeof(unsigned char), 1, input);
			(*img).mat[i][j] = (int)crt;
		}
	}
	fclose(input);
	selectietotala(img);
}

// extrag cuvintele citite dupa comanda
int cuvinte(cuvinte_p *c)
{
	char delim[] = " '\n'";
	int nr = 0;
	for (char *p = strtok((*c).prop, delim); p; p = strtok(NULL, delim)) {
		strcpy((*c).cuv[nr], p);
		nr++;
	}
	return nr;
}

// interschimb variabilele a si b intre ele
void interschimbare(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

// verific daca cuvantul citit este un numar
double cifreint(char c[])
{
	int n = strlen(c);
	char p[lg_max];
	int nr = 0;
	strcpy(p, c);
	// daca numarul citit e negativ
	if ((p[0] == '-')) {
		for (int i = 1; i < n; i++) {
			// verific daca nu am cifre
			if (p[i] > '9' || p[i] < '0')
				return 1. / 2;
		}
		for (int i = 1; i < n; i++)
			nr = nr * 10 + (c[i] - '0');
		return -nr;
	}
	// daca e pozitiv
	for (int i = 0; i < n; i++) {
		// verific daca nu am cifre
		if (p[i] > '9' || p[i] < '0')
			return 1. / 2;
	}
	for (int i = 0; i < n; i++)
		nr = nr * 10 + (c[i] - '0');
	return nr;
}

//selectez poza la comanda SELECT in functie de parametrii cititi
void select_poza(imagine *img)
{
	cuvinte_p c;
	fgets(c.prop, lg_max, stdin);
	int nr = cuvinte(&c);
	if ((*img).load == 0) {
		printf("No image loaded\n");
		return;
	}
	if (nr != 4 && nr != 1) {
		printf("Invalid command\n");
		return;
	}
	if (nr == 1 && !strcmp(c.cuv[0], "ALL") && (*img).load == 1) {
		selectietotala(img);
		printf("Selected ALL\n");
		return;
	} else if (nr == 1) {
		printf("Invalid command\n");
		return;
	}
	int crd[4];
	for (int i = 0; i < nr; i++) {
		if (cifreint(c.cuv[i]) == 1. / 2) {
			printf("Invalid command\n");
			return;
		} else if ((int)cifreint(c.cuv[i]) < 0) {
			printf("Invalid set of coordinates\n");
			return;
		}
		crd[i] = cifreint(c.cuv[i]);
	}
	if (crd[0] > crd[2])
		interschimbare(&crd[0], &crd[2]);
	if (crd[1] > crd[3])
		interschimbare(&crd[1], &crd[3]);
	if (crd[3] > (*img).lin || crd[2] > (*img).col) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (crd[0] == crd[2] || crd[1] == crd[3]) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (nr == 4 && (*img).load == 1) {
		(*img).x1 = crd[0];
		(*img).y1 = crd[1];
		(*img).x2 = crd[2];
		(*img).y2 = crd[3];
		printf("Selected %d %d %d %d\n", crd[0], crd[1], crd[2], crd[3]);
	}
}

// verific daca un numar este putere a lui 2 pentru functia histogram
int power2(int k)
{
	if (k <= 0)
		return 0;
	if (k == 1)
		return 1;
	if (k % 2 == 1)
		return 0;
	else
		return power2(k / 2);
}

// calculez un vector de frecventa pentru functiile histogram si equalize
void frecventa(imagine *img, int **f)
{
	int *h;
	h = (int *)calloc(256, sizeof(int));
	if (!h) {
		// daca a esuat alocarea
		return;
	}
	for (int i = 0; i < (*img).lin; i++)
		for (int j = 0; j < (*img).col; j++)
			h[(*img).mat[i][j]]++;
	(*f) = h;
}

// fac histograma pozei la comanda HISTOGRAM
void histograma(imagine img)
{
	cuvinte_p c;
	fgets(c.prop, lg_max, stdin);
	int nr = cuvinte(&c);
	if (img.load == 0) {
		printf("No image loaded\n");
		return;
	}
	if (nr != 2) {
		printf("Invalid command\n");
		return;
	}
	for (int i = 0; i < nr; i++)
		if (cifreint(c.cuv[i]) < 0) {
			printf("Invalid command\n");
			return;
		}
	if (nr == 2 && img.load == 1) {
		if (img.rgb == 3) {
			printf("Black and white image needed\n");
			return;
		}
		int x, y;
		x = cifreint(c.cuv[0]);
		y = cifreint(c.cuv[1]);
		if (power2(y) == 0 && y < 0) {
			printf("Invalid set of parameters\n");
			return;
		}
		int *f;
		frecventa(&img, &f);
		int *s, nr = 0;
		s = (int *)calloc(256, sizeof(int));
		if (!s) {
			// daca a esuat alocarea
			return;
		}
		// calculez suma fiecarei grupare de binuri
		for (int k = 0; k < img.max_pix + 1; k += (img.max_pix + 1) / y) {
			for (int i = k; i < k + (img.max_pix + 1) / y; i++)
				s[nr] += f[i];
			nr++;
		}
		// calculez maximul dintre sumele frecventelor
		int max = 0;
		for (int i = 0; i < nr; i++)
			if (max < s[i])
				max = s[i];
		for (int i = 0; i < nr; i++)
			s[i] = round((s[i] * x) / max);
		for (int i = 0; i < y; i++) {
			printf("%d	|	", s[i]);
			for (int j = 0; j < s[i]; j++)
				printf("*");
			printf("\n");
		}
		free(f);
		free(s);
	}
}

// cropez imaginea dupa selectia curenta
void crop(imagine *img)
{
	cuvinte_p c;
	fgets(c.prop, lg_max, stdin);
	int nr = cuvinte(&c);
	if ((*img).load == 0) {
		printf("No image loaded\n");
		return;
	} else if (nr != 0) {
		printf("Invalid command\n");
		return;
	}
	int **aux;
	int ln, cn;
	ln = (*img).y2 - (*img).y1;
	cn = ((*img).x2 - (*img).x1);
	alocare_matrice(ln, cn, &aux, (*img).rgb);
	for (int i = 0; i < ln; i++)
		for (int j = 0; j < ((*img).rgb * cn); j++)
			aux[i][j] = (*img).mat[i + (*img).y1][j + (*img).rgb * (*img).x1];
	free_image(img);
	(*img).mat = aux;
	(*img).lin = ln;
	(*img).col = cn;
	selectietotala(img);
	printf("Image cropped\n");
}

// incarc in memorie o poza din fisier
void load(imagine *img)
{
	FILE *input;
	if ((*img).load == 1)
		free_image(img);
	cuvinte_p c;
	fgets(c.prop, lg_max, stdin);
	int nr = cuvinte(&c);
	if (nr != 1) {
		printf("Invalid command\n");
		(*img).load = 0;
		return;
	}
	input = fopen(c.cuv[0], "rb");
	if (!input) {
		printf("Failed to load %s\n", c.cuv[0]);
		(*img).load = 0;
		return;
	}
	fscanf(input, "%s\n", (*img).tip);
	ignorare_com(input);
	fscanf(input, "%d %d\n", &(*img).col, &(*img).lin);
	fscanf(input, "%d\n", &(*img).max_pix);
	// in functie de formatul fiecarei poze aplic citirea respectiva
	if (!strcmp((*img).tip, "P2")) {
		// citesc poze gri ascii
		(*img).rgb = 1;
		citire_poza_ascii(img, input);
	} else if (!strcmp((*img).tip, "P5")) {
		// citesc poze grii binar
		(*img).rgb = 1;
		citire_poza_binar(img, input);
	} else if (!strcmp((*img).tip, "P3")) {
		// citesc poze rgb ascii
		(*img).rgb = 3;
		citire_poza_ascii(img, input);
	} else if (!strcmp((*img).tip, "P6")) {
		// citesc poze rgb binar
		(*img).rgb = 3;
		citire_poza_binar(img, input);
	}
	(*img).load = 1;
	printf("Loaded %s\n", c.cuv[0]);
}

// fac equalize pentru pozele gri
void equalize(imagine *img)
{
	cuvinte_p c;
	fgets(c.prop, lg_max, stdin);
	int nr = cuvinte(&c);
	if ((*img).load == 0) {
		printf("No image loaded\n");
		return;
	} else if (nr != 0) {
		printf("Invalid command\n");
		return;
	}
	if ((*img).rgb == 3) {
		printf("Black and white image needed\n");
		return;
	}
	// calculez frecventa
	int *h;
	frecventa(img, &h);
	long long area;
	area = (*img).lin * (*img).col;
	int *s;
	s = (int *)calloc(256, sizeof(int));
		if (!s) {
			// daca a esuat alocarea
			return;
		}
	// calculez suma fiecarei valori a unui pixel pentru a nu retine o copie
	// a imagini cand modific pixelul cu valoarea obtinuta dupa aplicarea
	// formulei
	for (int i = 0; i < 256; i++)
		for (int k = 0; k <= i; k++)
			s[i] += h[k];
	// inlocuiesc valoarea corespunzatoarea pixelului cu cea din suma calculata
	for (int i = 0; i < (*img).lin; i++)
		for (int j = 0; j < (*img).col; j++) {
			int m = (*img).mat[i][j];
			m = round((double)(s[m] * 255 * 1.) / area);
			m = clamp(m, 0, 255);
			(*img).mat[i][j] = m;
		}
	// eliberez vectorii folositi
	free(s);
	free(h);
	printf("Equalize done\n");
}

// verific daca am selectie totala pentru rotate
int verif_selectietotala(imagine *img)
{
	if ((*img).x1 == 0 && (*img).x2 == (*img).col &&
		(*img).y1 == 0 && (*img).y2 == (*img).lin)
		return 1;
	return 0;
}

// rotesc la stanga pentru poze gri
void rotate_90_stanga_gri(imagine *img)
{
	int **aux;
	int ln, cn;
	ln = (*img).y2 - (*img).y1;
	cn = ((*img).x2 - (*img).x1);
	alocare_matrice(cn, ln, &aux, (*img).rgb);
	for (int i = 0; i < cn; i++)
		for (int j = 0; j < ln; j++)
			aux[i][j] = (*img).mat[j + (*img).y1][(*img).x2 - i - 1];
	// daca am selectie totala schimb indicii intre ei
	if (verif_selectietotala(img) == 1) {
		free_image(img);
		(*img).mat = aux;
		interschimbare(&(*img).lin, &(*img).col);
		interschimbare(&(*img).y2, &(*img).x2);
		interschimbare(&(*img).x1, &(*img).y1);
		return;
	}
	// modific imaginea rotita daca nu am selectie totala
	for (int i = 0; i < ln; i++)
		for (int j = 0; j < cn; j++)
			(*img).mat[i + (*img).y1][j + (*img).x1] = aux[i][j];
	// eliberez matricea folosita pentru rotate
	free_mat(aux, cn);
}

// rotesc la stanga pentru poze rgb
void rotate_90_stanga_rgb(imagine *img)
{
	int **aux;
	int ln, cn;
	ln = (*img).y2 - (*img).y1;
	cn = ((*img).x2 - (*img).x1);
	alocare_matrice(cn, ln, &aux, 3);
	for (int i = 0; i < cn; i++)
		for (int j = 0; j < ln; j++)
			for (int k = 0; k < 3; k++)
				aux[i][3 * j + k] = (*img).mat[j + (*img).y1]
				[3 * ((*img).x2 - i - 1) + k];
	// daca am selectie totala modific indicii
	if (verif_selectietotala(img) == 1) {
		free_image(img);
		(*img).mat = aux;
		interschimbare(&(*img).lin, &(*img).col);
		interschimbare(&(*img).y2, &(*img).x2);
		interschimbare(&(*img).x1, &(*img).y1);
		return;
	}
	// daca nu am selectie totala modific pixeli rotiti
	for (int i = 0; i < ln; i++)
		for (int j = 0; j < 3 * cn; j++)
			(*img).mat[i + (*img).y1][j + 3 * (*img).x1] = aux[i][j];
	// eliberez matricea folosita pentru rotate
	free_mat(aux, cn);
}

void rotate(imagine *img)
{
	cuvinte_p c;
	fgets(c.prop, lg_max, stdin);
	int nr = cuvinte(&c);
	if ((*img).load == 0) {
		printf("No image loaded\n");
		return;
	}
	if (nr != 1) {
		printf("Invalid command\n");
		return;
	}
	int unghi;
	if (cifreint(c.cuv[0]) == 1. / 2) {
		printf("Invalid command\n");
		return;
	}
	unghi = cifreint(c.cuv[0]);
	if (unghi % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return;
	}
	if (verif_selectietotala(img) == 0) {
		if (((*img).y2 - (*img).y1) != ((*img).x2 - (*img).x1)) {
			printf("The selection must be square\n");
			return;
		}
	}
	int raport;
	raport = unghi / 90;
	for (int k = 1; k <= 4; k++)
		if (raport % 4 == k || raport % 4 == (k - 4)) {
			// pentru ca am facut doar o functie pentru rotirea la stanga
			// in variabila nr retin numarul de rotatii al unei poze
			nr = 4 - k;
			for (int i = 1; i <= nr; i++)
				if ((*img).rgb == 1)
					rotate_90_stanga_gri(img);
				else
					rotate_90_stanga_rgb(img);
		}
	printf("Rotated %d\n", unghi);
}

// atribui pentru APPLY EDGE matricea specifica
void edge(int ***mat, int *frc)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			(*mat)[i][j] = -1;
	(*mat)[1][1] = 8;
	(*frc) = 1;
}

// atribui pentru APPLY SHARPEN matricea specifica
void sharpen(int ***mat, int *frc)
{
	(*mat)[0][0] = 0;
	(*mat)[0][1] = -1;
	(*mat)[0][2] = 0;
	(*mat)[1][0] = -1;
	(*mat)[1][1] = 5;
	(*mat)[1][2] = -1;
	(*mat)[2][0] = 0;
	(*mat)[2][1] = -1;
	(*mat)[2][2] = 0;
	(*frc) = 1;
}

// atribui pentru APPLY BLUR matricea specifica
void blur(int ***mat, int *frc)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			(*mat)[i][j] = 1;
	(*frc) = 9;
}

// atribui pentru APPLY GAUSSIAN_BLUR matricea specifica
void g_blur(int ***mat, int *frc)
{
	(*mat)[0][0] = 1;
	(*mat)[0][1] = 2;
	(*mat)[0][2] = 1;
	(*mat)[1][0] = 2;
	(*mat)[1][1] = 4;
	(*mat)[1][2] = 2;
	(*mat)[2][0] = 1;
	(*mat)[2][1] = 2;
	(*mat)[2][2] = 1;
	(*frc) = 16;
}

void apply(imagine *img)
{
	cuvinte_p c;
	fgets(c.prop, lg_max, stdin);
	int nr = cuvinte(&c);
	if ((*img).load == 0) {
		printf("No image loaded\n");
		return;
	}
	if (nr != 1) {
		printf("Invalid command\n");
		return;
	}
	if ((*img).rgb == 1) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	int **mat = (int **)malloc(3 * sizeof(int *));
	if (!mat) {
		// daca esueaza alocarea
		return;
	}
	for (int i = 0; i < 3; i++) {
		mat[i] = (int *)malloc(3 * sizeof(int));
		if (!mat[i]) {
			// daca esueaza alocarea
			for (int j = 0; j < i; j++)
				free(mat[j]);
			free(mat);
			return;
		}
	}
	// frac il folosesc pentru a imparti la valoarea specifica apply-ului
	// selectat in functie de parametru
	int frac;
	if (!strcmp(c.cuv[0], "EDGE")) {
		edge(&mat, &frac);
	} else if (!strcmp(c.cuv[0], "SHARPEN")) {
		sharpen(&mat, &frac);
	} else if (!strcmp(c.cuv[0], "BLUR")) {
		blur(&mat, &frac);
	} else if (!strcmp(c.cuv[0], "GAUSSIAN_BLUR")) {
		g_blur(&mat, &frac);
	} else {
		printf("APPLY parameter invalid\n");
		return;
	}
	// calculez pentru fiecare culoare suma vecinilor inmultita
	// corespunzator cu matricea specifica si impartita la frac
	int **aux;
	int ln, cn;
	ln = (*img).y2 - (*img).y1;
	cn = ((*img).x2 - (*img).x1);
	alocare_matrice(ln, cn, &aux, 3);
	for (int i = 0; i < ln; i++)
		for (int j = 0; j < 3 * cn; j += 3) {
			aux[i][j] =
			suma_vecin(img, i + (*img).y1, j + 3 * (*img).x1, mat, 0, frac);
			aux[i][j + 1] =
			suma_vecin(img, i + (*img).y1, j + 3 * (*img).x1, mat, 1, frac);
			aux[i][j + 2] =
			suma_vecin(img, i + (*img).y1, j + 3 * (*img).x1, mat, 2, frac);
		}
	for (int i = 0; i < ln; i++)
		for (int j = 0; j < 3 * cn; j++)
			(*img).mat[i + (*img).y1][j + (*img).rgb * (*img).x1] = aux[i][j];
	// eliberez matricile folosite
	free_mat(mat, 3);
	free_mat(aux, ln);
	printf("APPLY %s done\n", c.cuv[0]);
}

// salvez in format ascii orice poza
void save_ascii(imagine img, FILE *out)
{
	if (img.rgb == 3)
		fprintf(out, "P3\n");
	else
		fprintf(out, "P2\n");
	fprintf(out, "%d %d\n", img.col, img.lin);
	fprintf(out, "%d\n", img.max_pix);
	for (int i = 0; i < img.lin; i++) {
		for (int j = 0; j < img.rgb * img.col; j++)
			fprintf(out, "%d ", img.mat[i][j]);
		fprintf(out, "\n");
	}
	fclose(out);
}

// salvez in format binar orice poza
void save_binar(imagine img, FILE *out)
{
	if (img.rgb == 3)
		fprintf(out, "P6\n");
	else
		fprintf(out, "P5\n");
	fprintf(out, "%d %d\n", img.col, img.lin);
	fprintf(out, "%d\n", img.max_pix);
	for (int i = 0; i < img.lin; i++)
		for (int j = 0; j < img.rgb * img.col; j++)
			fwrite(&img.mat[i][j], sizeof(unsigned char), 1, out);
	fclose(out);
}

void save(imagine img)
{
	cuvinte_p c;
	fgets(c.prop, lg_max, stdin);
	if (img.load == 0) {
		printf("No image loaded\n");
		return;
	}
	FILE *out;
	int nr = cuvinte(&c);
	// int nr = sscanf(c.prop, "%s %s", c.cuv[0], c.cuv[1]);
	if (nr != 1 && nr != 2) {
		printf("Invalid command\n");
		return;
	}
	if (nr == 2 && !strcmp(c.cuv[1], "ascii")) {
		out = fopen(c.cuv[0], "w");
		// fprintf(out, "%s\n", img.tip);
		save_ascii(img, out);
		printf("Saved %s\n", c.cuv[0]);
	} else if (nr == 1) {
		out = fopen(c.cuv[0], "wb");
		save_binar(img, out);
		printf("Saved %s\n", c.cuv[0]);
	} else {
		printf("Invalid command\n");
	}
}

// eliberez matricea pozei dupa comanda exit
void exit_poza(imagine *img, int *citire)
{
	if ((*img).load == 1) {
		free_image(img);
			(*citire) = 0;
	} else if ((*img).load == 0) {
		printf("No image loaded\n");
			(*citire) = 0;
	}
}

int main(void)
{
	imagine img;
	char comanda[lg_max];
	declarare(&img);
	int citire = 1;
	while (citire) {
		scanf("%s", comanda);
		if (!strcmp(comanda, "LOAD")) {
			load(&img);
		} else if (!strcmp(comanda, "SELECT")) {
			select_poza(&img);
		} else if (!strcmp(comanda, "APPLY")) {
			apply(&img);
		} else if (!strcmp(comanda, "HISTOGRAM")) {
			histograma(img);
		} else if (!strcmp(comanda, "EQUALIZE")) {
			equalize(&img);
		} else if (!strcmp(comanda, "ROTATE")) {
			rotate(&img);
		} else if (!strcmp(comanda, "CROP")) {
			crop(&img);
		} else if (!strcmp(comanda, "SAVE")) {
			save(img);
		} else if (!strcmp(comanda, "EXIT")) {
			exit_poza(&img, &citire);
		} else {
			cuvinte_p c;
			fgets(c.prop, lg_max, stdin);
			printf("Invalid command\n");
		}
	}
	return 0;
}
