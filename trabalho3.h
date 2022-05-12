/*============================================================================*/
/* MÓDULO QUE BUSCA O CAMINHO MAIS CURTO PARA SAIR DE UM LABIRINTO            */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/

#ifndef __TRABALHO3_H
#define __TRABALHO3_H

/*============================================================================*/

#include "imagem.h"

/*============================================================================*/
/* Um tipo simples, usado para representar uma coordenada. */

typedef struct
{
    int x;
    int y;
} Coordenada;

/*============================================================================*/
/* Função central do trabalho. */

int encontraCaminho (Imagem1C* img, Coordenada** caminho);

/*============================================================================*/
#endif /* __TRABALHO3_H */

