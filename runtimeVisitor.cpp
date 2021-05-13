#include <iostream>
#include <string>
#include <exception>
#include "runtimeVisitor.h"

using namespace std;

antlrcpp::Any runtimeVisitor::visitId_list(pascalParser::Id_listContext *ctx) {
    // Prima variabile nella lista di identificatori
    string varname = ctx->ID()->getText();
    // Resto della lista
    pascalParser::Id_listContext *tail = ctx->id_list();
    // controllo che la variabile non sia già stata dichiarata
    if(this->vars.find(varname) != this->vars.end()) {
        cerr << "Error: Duplicate variable declaration '" << varname << "'" << endl;
        exit(EXIT_FAILURE);
    }
    this->vars[varname] = 0;
    // Continua sul resto della lista di identificatori, se presente
    if(tail != NULL)
        return visitId_list(tail);
    return NULL;
}

antlrcpp::Any runtimeVisitor::visitAssign(pascalParser::AssignContext *ctx) {
    string varname = ctx->ID()->getText();
    // controllo che la variabile sia stata dichiarata
    if(this->vars.find(varname) == this->vars.end()) {
        cerr << "Error: Undefined variable '" << varname << "'" << endl;
        exit(EXIT_FAILURE);
    }
    int value = visitExpr(ctx->expr());
    this->vars[varname] = value;
    return NULL;
}

antlrcpp::Any runtimeVisitor::visitOut(pascalParser::OutContext *ctx) {
    // verifico se devo stampare intero o stringa
    if(ctx->expr() != NULL) {
        // caso stampa intero
        int value = visitExpr(ctx->expr());
        cout << value << endl;
    }else if(ctx->STRING() != NULL){
        string value = ctx->STRING()->getText();
        value = value.substr(1, value.length() - 2);
        cout << value << endl;
    }
    
    return NULL;
}

antlrcpp::Any runtimeVisitor::visitIn(pascalParser::InContext *ctx) {
    // TODO: implementa la lettura dell'input da tastiera
    string varname = ctx->ID()->getText();
    // controllo che la variabile sia stata dichiarata
    if(this->vars.find(varname) == this->vars.end()) {
        cerr << "Error: Undefined variable '" << varname << "'" << endl;
        exit(EXIT_FAILURE);
    }
    int value;
    cin >> value;
    this->vars[varname] = value;

    return NULL;
}


antlrcpp::Any runtimeVisitor::visitBranch(pascalParser::BranchContext *ctx) {
    // stabilisce il valore della guardia
    bool guard = visitGuard(ctx->guard());
    if(guard) {
        // se guardia vera, esegue ramo then
        visitCode_block(ctx->code_block(0));
    }
    // TODO: implementa l'esecuzione del ramo else (se presente) quando la guardia è falsa
    else
    {
        if(ctx->code_block(1)!=NULL)
         visitCode_block(ctx->code_block(1));
    }

    return NULL;
}

antlrcpp::Any runtimeVisitor::visitLoop(pascalParser::LoopContext *ctx) {
    // TODO: implementa l'esecuzione del ciclo repeat-until
    bool guard;
    do{
        visitSt_list(ctx->st_list());
        guard = visitGuard(ctx->guard());
    }while(guard == false);

    return NULL;
}

antlrcpp::Any runtimeVisitor::visitExpr(pascalParser::ExprContext *ctx) {
    // controllo in quale caso sono
    int value = 0;
    if(ctx->ID() != NULL) {
        // caso ID
        string varname = ctx->ID()->getText();
        if(this->vars.find(varname) == this->vars.end()) {
            cerr << "Error: Undefined variable '" << varname << "'" << endl;
            exit(EXIT_FAILURE);
        }
        value = this->vars[varname];
    } else if(ctx->NUMBER() != NULL) {
        // caso NUMBER
        string numtext = ctx->NUMBER()->getText();
        value = std::stoi(numtext);
    } else if(ctx->PLUS() != NULL) {
        // caso expr + expr
        // calcolo il valore della prima expr
        int left = visitExpr(ctx->expr(0));
        // calcolo il valore della seconda expr
        int right = visitExpr(ctx->expr(1));
        // il risultato finale è la somma
        value = left + right;
    } else if(ctx->MINUS() != NULL) {
        // caso expr - expr
        // calcolo il valore della prima expr
        int left = visitExpr(ctx->expr(0));
        // calcolo il valore della seconda expr
        int right = visitExpr(ctx->expr(1));
        // il risultato finale è la differenza
        value = left - right;
    } else if(ctx->MULT() != NULL) {
        // caso expr * expr
        // calcolo il valore della prima expr
        int left = visitExpr(ctx->expr(0));
        // calcolo il valore della seconda expr
        int right = visitExpr(ctx->expr(1));
        // il risultato finale è la moltiplicazione
        value = left * right;
    } else if(ctx->DIV() != NULL) {
        // caso expr / expr
        // calcolo il valore della prima expr
        int left = visitExpr(ctx->expr(0));
        // calcolo il valore della seconda expr
        int right = visitExpr(ctx->expr(1));
        // il risultato finale è la divisione
        value = left / right;
    } else if(ctx->MOD() != NULL) {
        // caso expr % expr
        // calcolo il valore della prima expr
        int left = visitExpr(ctx->expr(0));
        // calcolo il valore della seconda expr
        int right = visitExpr(ctx->expr(1));
        // il risultato finale è il modulo
        value = left % right;
    } else if(ctx->expr().size() == 1) {
        // caso parentesi
        value = visitExpr(ctx->expr(0));
    }
    return value;
}

antlrcpp::Any runtimeVisitor::visitGuard(pascalParser::GuardContext *ctx) {
    // TODO: implementa la valutazione di una espressione booleana
    // il metodo ritorna true se l'espressione è vera, false altrimenti

 // controllo in quale caso sono
    bool value = false;
    if(ctx->NOT() != NULL) {
        // caso NOT
        value = !visitGuard(ctx->guard(0));
    } else if(ctx->AND() != NULL) {
       // calcolo il valore della prima guard
        bool left = visitGuard(ctx->guard(0));
        // calcolo il valore della seconda guard
        bool right = visitGuard(ctx->guard(1));

        if(left && right)
            value = true;
        else
            value = false;

    } else if(ctx->OR() != NULL) {
       // calcolo il valore della prima guard
        bool left = visitGuard(ctx->guard(0));
        // calcolo il valore della seconda guard
        bool right = visitGuard(ctx->guard(1));

        if(left || right)
            value = true;
        else
            value = false;
    }  else if(ctx->relation() != NULL) {
        // caso relation

        //da rivedere
        value = visitRelation(ctx->relation());
    }  else if(ctx->guard().size() == 1) {
        // caso parentesi
        value = visitGuard(ctx->guard(0));
    }

    return value;
}

antlrcpp::Any runtimeVisitor::visitRelation(pascalParser::RelationContext *ctx) {
    // TODO: implementa la valutazione di un confronto
    // il metodo ritorna true se il confronto è vero, false altrimenti
    bool value = false;

    if (ctx->LT() != NULL) {
        // calcolo il valore della prima expr
        int left = visitExpr(ctx->expr(0));
        // calcolo il valore della seconda expr
        int right = visitExpr(ctx->expr(1));
        // il risultato finale è true se left < right
        value = left < right;
    }
    else if (ctx->LEQ() != NULL) {
        // calcolo il valore della prima expr
        int left = visitExpr(ctx->expr(0));
        // calcolo il valore della seconda expr
        int right = visitExpr(ctx->expr(1));
        // il risultato finale è true se left <= right
        value = left <= right;
    }
    else if (ctx->EQ() != NULL) {
        // calcolo il valore della prima expr
        int left = visitExpr(ctx->expr(0));
        // calcolo il valore della seconda expr
        int right = visitExpr(ctx->expr(1));
        // il risultato finale è true se left == right
        value = left == right;
    }
    else if (ctx->NEQ() != NULL)
    {
        // calcolo il valore della prima expr
        int left = visitExpr(ctx->expr(0));
        // calcolo il valore della seconda expr
        int right = visitExpr(ctx->expr(1));
        // il risultato finale è true se left != right
        value = left != right;
    }
    else if (ctx->GEQ() != NULL) {
        // calcolo il valore della prima expr
        int left = visitExpr(ctx->expr(0));
        // calcolo il valore della seconda expr
        int right = visitExpr(ctx->expr(1));
        // il risultato finale è true se left >= right
        value = left >= right;
    }
    else if (ctx->GT() != NULL) {
        // calcolo il valore della prima expr
        int left = visitExpr(ctx->expr(0));
        // calcolo il valore della seconda expr
        int right = visitExpr(ctx->expr(1));
        // il risultato finale è true se left > right
        value = left > right;
    }
    
    return value;
}

