%uview typecheck; 
/* typechecking */

%{ KC_UNPARSE
#include "diagnosis.h" //TODO: Location von yyerror hier nicht korrekt
%}

/* Namen nicht drucken */

NetName(*) -> [typecheck: ];
Sort(typeS, *, arrayL) -> [typecheck: typeS arrayL];
TypeEnum(identL) -> [typecheck: identL];
IdentList(identL, id) -> [typecheck: identL id];
EmptyIdentList() -> [typecheck: ];
TypeIdent(*) -> [typecheck: ];
StructType(typeS, *, arrayL) -> [typecheck: typeS arrayL];
VarOrArray(*, arrayL) -> [typecheck: arrayL];
Function(typeF, *, arrayL, functionParam, blocks) -> [typecheck: typeF arrayL functionParam blocks];
Constant(typeS, *, arrayL, expr) -> [typecheck: typeS arrayL expr];
LeftValIdent(*) -> [typecheck: ];
LeftValDot(leftval, *) -> [typecheck: leftval];
PlaceTypeEnum(*, optNumb, identL) -> [typecheck: optNumb identL];
Place(*) -> [typecheck: ];
PlaceWithTypeIdent(*, *) -> [typecheck: ];
Marking(*, expr, markingL) -> [typecheck: expr markingL];
MarkingList(*, expr, markingL) -> [typecheck: expr markingL];
Transition(*, fair, var, optGuard, optIdentExprL1, optIdentExprL2, transitions) -> [typecheck: fair var optGuard optIdentExprL1 optIdentExprL2 transitions];
OptIdentExprList(*, expr, identExprL) -> [typecheck: expr identExprL]; 
IdentExprList(*, expr, identExprL) -> [typecheck: expr identExprL];
StatementForTypeExpr(typeS, *, expr1, expr2, expr3, statem) -> [typecheck: typeS expr1 expr2 expr3 statem];
StatementForIdentColon(*, expr, statem) -> [typecheck: expr statem];
StatementForAll(type1, *, type2) -> [typecheck: type1 type2];


/*--------- type ---------*/

TypeIntInterval(expr1, expr2) -> [typecheck: expr1 expr2
	//TODO { if(expr1->value > expr2->value) yyerror("Lower interval boundary greater than upper"); }
	// wenn beide arithmetisch, sonst muss noch Idents checken zb bei int(1,N)
];


/*--------- expression ---------*/

expr = ExprAssignment(assign) -> [typecheck: assign
	{ expr->check=assign->check; /*TODO:?*/}
];

expr = ExprArithmetic(arithm) -> [typecheck: arithm
	{ if(arithm->check==NUMB) { expr->check=NUMB; }
	  else { yyerror("Non-Arithmetic"); }
	}
];

expr = ExprLogical(logic) -> [typecheck: logic
	{ if(logic->check==BOOL) { expr->check=BOOL; }
	  else { yyerror("Non-Bool"); }
	}
];

expr = ExprComparison(compare) -> [typecheck: compare
	{ if(compare->check==BOOL) { expr->check=BOOL; }
	  else { yyerror("Non-Bool"); }
	}
];

/* expr = ExprOther(others) -> [typecheck: others];
*/


/*--------- expessionWithoutInitializer ---------*/

exprInit = ExprAssignment2(assign) -> [typecheck: assign
	{ exprInit->check=assign->check; /*TODO:?*/}
];

exprInit = ExprArithmeticWithoutInitializer(arithmInit) -> [typecheck: arithmInit
	{ if(arithmInit->check==NUMB) { exprInit->check=NUMB; }
	  else { yyerror("Non-Arithmetic"); }
	}
];

exprInit = ExprLogicalWithoutInitializer(logicInit) -> [typecheck: logicInit
	{ if(logicInit->check==BOOL) { exprInit->check=BOOL; }
	  else { yyerror("Non-Bool"); }
	}
];

exprInit = ExprComparisonWithoutInitializer(compareInit) -> [typecheck: compareInit
	{ if(compareInit->check==BOOL) { exprInit->check=BOOL; }
	  else { yyerror("Non-Bool"); }
	}
];

/* exprInit = ExprOtherWithoutInitializer(othersInit) -> [typecheck: othersInit];
*/


/*--------- assignment ---------*/

/*  AssignEqual(leftvalue expression) TODO*/

assign = AssignPlus(leftval, expr) -> [typecheck: leftval expr
	{ if(expr->check==NUMB /*&& leftval->check==NUMB*/) { assign->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

assign = AssignMinus(leftval, expr) -> [typecheck: leftval expr
	{ if(expr->check==NUMB /*&& leftval->check==NUMB*/) { assign->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

assign = AssignTimes(leftval, expr) -> [typecheck: leftval expr
	{ if(expr->check==NUMB /*&& leftval->check==NUMB*/) { assign->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

assign = AssignDivide(leftval, expr) -> [typecheck: leftval expr
	{ if(expr->check==NUMB /*&& leftval->check==NUMB*/) { assign->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

assign = AssignMod(leftval, expr) -> [typecheck: leftval expr
	{ if(expr->check==NUMB /*&& leftval->check==NUMB*/) { assign->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

/*--------- arithmetic ---------*/

arithm = PositiveExpr(expr) -> [typecheck: expr
	{ if(expr->check==NUMB) { arithm->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

arithm = NegativeExpr(expr) -> [typecheck: expr
	{ if(expr->check==NUMB) { arithm->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

arithm = ExprAddition(expr1, expr2) -> [typecheck: expr1 expr2
	{ if(expr1->check==NUMB && expr2->check==NUMB) { arithm->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

arithm = ExprSubtraction(expr1, expr2) -> [typecheck: expr1 expr2
	{ if(expr1->check==NUMB && expr2->check==NUMB) { arithm->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

arithm = ExprMultiplication(expr1, expr2) -> [typecheck: expr1 expr2
	{ if(expr1->check==NUMB && expr2->check==NUMB) { arithm->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

arithm = ExprDivision(expr1, expr2) -> [typecheck: expr1 expr2
	{ if(expr1->check==NUMB && expr2->check==NUMB) { arithm->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

arithm = ExprModulo(expr1, expr2) -> [typecheck: expr1 expr2
	{ if(expr1->check==NUMB && expr2->check==NUMB) { arithm->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

arithm = ExprNumber(*) -> [typecheck: { arithm->check=NUMB; fprintf(stdout,"NUMB!\n");} ];


/*--------- arithmetikWithoutInitializer ---------*/

arithmInit = PositiveExpr2(expr) -> [typecheck: expr
	{ if(expr->check==NUMB) { arithmInit->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

arithmInit = NegativeExpr2(expr) -> [typecheck: expr
	{ if(expr->check==NUMB) { arithmInit->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

arithmInit = ExprAdditionWithoutInitializer(exprInit, expr2) -> [typecheck: exprInit expr2
	{ if(exprInit->check==NUMB && expr2->check==NUMB) { arithmInit->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

arithmInit = ExprSubtractionWithoutInitializer(exprInit, expr2)  -> [typecheck: exprInit expr2
	{ if(exprInit->check==NUMB && expr2->check==NUMB) { arithmInit->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

arithmInit = ExprMultiplicationWithoutInitializer(exprInit, expr2)  -> [typecheck: exprInit expr2
	{ if(exprInit->check==NUMB && expr2->check==NUMB) { arithmInit->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

arithmInit = ExprDivisionWithoutInitializer(exprInit, expr2)  -> [typecheck: exprInit expr2
	{ if(exprInit->check==NUMB && expr2->check==NUMB) { arithmInit->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

arithmInit = ExprModuloWithoutInitializer(exprInit, expr2)  -> [typecheck: exprInit expr2
	{ if(exprInit->check==NUMB && expr2->check==NUMB) { arithmInit->check=NUMB; }
	  else { yyerror("Non-arithmetic"); }
	}
];

arithmInit = ExprNumber2(*) -> [typecheck: { arithmInit->check=NUMB; fprintf(stdout,"\nNUMB!\n");} ];


/*--------- logical ---------*/

logic = NotExpr(expr) -> [typecheck: expr
	{ if(expr->check==BOOL) { logic->check=BOOL; }
	  else { yyerror("Non-Bool"); }
	}
];

logic = ExprAnd(expr1,expr2) -> [typecheck: expr1 expr2
	{ if(expr1->check==BOOL && expr2->check==BOOL) { logic->check=BOOL; }
	  else { yyerror("Non-Bool"); }
	}
];

logic = ExprOr(expr1,expr2) -> [typecheck: expr1 expr2
	{ if(expr1->check==BOOL && expr2->check==BOOL) { logic->check=BOOL; }
	  else { yyerror("Non-Bool"); }
	}
];

logic = ExprTrue() -> [typecheck:
	{ logic->check=BOOL; fprintf(stdout,"BOOL!\n"); }
];

logic = ExprFalse() -> [typecheck:
	{ logic->check=BOOL; fprintf(stdout,"BOOL!\n"); }
];

/*--------- logicalWithoutInitializer ---------*/

logicInit = NotExpr2(expr) -> [typecheck: expr
	{ if(expr->check==BOOL) { logicInit->check=BOOL; }
	  else { yyerror("Non-Bool"); }
	}
];

logicInit = ExprAndWithoutInitializer(exprInit,expr2) -> [typecheck: exprInit expr2
	{ if(exprInit->check==BOOL && expr2->check==BOOL) { logicInit->check=BOOL; }
	  else { yyerror("Non-Bool"); }
	}
];

logicInit = ExprOrWithoutInitializer(exprInit,expr2) -> [typecheck: exprInit expr2
	{ if(exprInit->check==BOOL && expr2->check==BOOL) { logicInit->check=BOOL; }
	  else { yyerror("Non-Bool"); }
	}
];

logicInit = ExprTrue2() -> [typecheck:
	{ logicInit->check=BOOL; fprintf(stdout,"\nBOOL!\n"); }
];

logicInit = ExprFalse2() -> [typecheck:
	{ logicInit->check=BOOL; fprintf(stdout,"\nBOOL!\n"); }
];

/*--------- comparison ---------*/

compare = ExprEquivalent(expr1, expr2) -> [typecheck: expr1 expr2
	{ if(expr1->check==expr2->check ||
	    (expr1->check==BOOL && expr2->check==NUMB) ||
	    (expr1->check==NUMB && expr2->check==BOOL)) /*TODO: ok? weitere?*/
	    { compare->check=BOOL; }
	  else { yyerror("Comparison between unmatchable types"); }
	}
];

compare = ExprNotEqual(expr1, expr2) -> [typecheck: expr1 expr2
	{ if(expr1->check==expr2->check ||
	    (expr1->check==BOOL && expr2->check==NUMB) ||
	    (expr1->check==NUMB && expr2->check==BOOL)) /*TODO: ok? weitere?*/
	    { compare->check=BOOL; }
	  else { yyerror("Comparison between unmatchable types"); }
	}
];

compare = ExprLessThan(expr1, expr2) -> [typecheck: expr1 expr2
	{ if(expr1->check==expr2->check ||
	    (expr1->check==BOOL && expr2->check==NUMB) ||
	    (expr1->check==NUMB && expr2->check==BOOL)) /*TODO: ok? weitere?*/
	    { compare->check=BOOL; }
	  else { yyerror("Comparison between unmatchable types"); }
	}
];

compare = ExprGreaterThan(expr1, expr2) -> [typecheck: expr1 expr2
	{ if(expr1->check==expr2->check ||
	    (expr1->check==BOOL && expr2->check==NUMB) ||
	    (expr1->check==NUMB && expr2->check==BOOL)) /*TODO: ok? weitere?*/
	    { compare->check=BOOL; }
	  else { yyerror("Comparison between unmatchable types"); }
	}
];

compare = ExprLessOrEqual(expr1, expr2) -> [typecheck: expr1 expr2
	{ if(expr1->check==expr2->check ||
	    (expr1->check==BOOL && expr2->check==NUMB) ||
	    (expr1->check==NUMB && expr2->check==BOOL)) /*TODO: ok? weitere?*/
	    { compare->check=BOOL; }
	  else { yyerror("Comparison between unmatchable types"); }
	}
];

compare = ExprGreaterOrEqual(expr1, expr2) -> [typecheck: expr1 expr2
	{ if(expr1->check==expr2->check ||
	    (expr1->check==BOOL && expr2->check==NUMB) ||
	    (expr1->check==NUMB && expr2->check==BOOL)) /*TODO: ok? weitere?*/
	    { compare->check=BOOL; }
	  else { yyerror("Comparison between unmatchable types"); }
	}
];

/*--------- comparisonWithoutInitializer ---------*/

compareInit = ExprEquivalentWithoutInitializer(exprInit, expr2) -> [typecheck: exprInit expr2
	{ if(exprInit->check==expr2->check ||
	    (exprInit->check==BOOL && expr2->check==NUMB) ||
	    (exprInit->check==NUMB && expr2->check==BOOL)) /*TODO: ok? weitere?*/
	    { compareInit->check=BOOL; }
	  else { yyerror("Comparison between unmatchable types"); }
	}
];

compareInit = ExprNotEqualWithoutInitializer(exprInit, expr2) -> [typecheck: exprInit expr2
	{ if(exprInit->check==expr2->check ||
	    (exprInit->check==BOOL && expr2->check==NUMB) ||
	    (exprInit->check==NUMB && expr2->check==BOOL)) /*TODO: ok? weitere?*/
	    { compareInit->check=BOOL; }
	  else { yyerror("Comparison between unmatchable types"); }
	}
];

compareInit = ExprLessThanWithoutInitializer(exprInit, expr2) -> [typecheck: exprInit expr2
	{ if(exprInit->check==expr2->check ||
	    (exprInit->check==BOOL && expr2->check==NUMB) ||
	    (exprInit->check==NUMB && expr2->check==BOOL)) /*TODO: ok? weitere?*/
	    { compareInit->check=BOOL; }
	  else { yyerror("Comparison between unmatchable types"); }
	}
];

compareInit = ExprGreaterThanWithoutInitializer(exprInit, expr2) -> [typecheck: exprInit expr2
	{ if(exprInit->check==expr2->check ||
	    (exprInit->check==BOOL && expr2->check==NUMB) ||
	    (exprInit->check==NUMB && expr2->check==BOOL)) /*TODO: ok? weitere?*/
	    { compareInit->check=BOOL; }
	  else { yyerror("Comparison between unmatchable types"); }
	}
];

compareInit = ExprLessOrEqualWithoutInitializer(exprInit, expr2) -> [typecheck: exprInit expr2
	{ if(exprInit->check==expr2->check ||
	    (exprInit->check==BOOL && expr2->check==NUMB) ||
	    (exprInit->check==NUMB && expr2->check==BOOL)) /*TODO: ok? weitere?*/
	    { compareInit->check=BOOL; }
	  else { yyerror("Comparison between unmatchable types"); }
	}
];

compareInit = ExprGreaterOrEqualWithoutInitializer(exprInit, expr2) -> [typecheck: exprInit expr2
	{ if(exprInit->check==expr2->check ||
	    (exprInit->check==BOOL && expr2->check==NUMB) ||
	    (exprInit->check==NUMB && expr2->check==BOOL)) /*TODO: ok? weitere?*/
	    { compareInit->check=BOOL; }
	  else { yyerror("Comparison between unmatchable types"); }
	}
];

/*--------- other ---------*/

others = ExprIdentParenthesis(string) -> [typecheck:
	{/* others->check=string....?; TODO*/}
];

others = ExprIdentExprInParenthesis(string, expr) -> [typecheck: expr
	{/* others->check=string....?; TODO*/}
];

/*ExprCommaSeparated(expression expression)
*/

others = ExprOtherIf(expr1, expr2, expr3) -> [typecheck: expr1 expr2 expr3
	{ if(expr1->check!=BOOL) { yyerror("Non-Bool"); } /*TODO: weiter..?*/
	}
];

/*| ExprAll(type)
*/

/*--------- otherWithoutInitializer ---------*/

othersInit = ExprIdentParenthesis2(string) -> [typecheck:
	{/* othersInit->check=string....?; TODO*/}
];

othersInit = ExprIdentExprInParenthesis2(string, expr) -> [typecheck: expr
	{/* othersInit->check=string....?; TODO*/}
];

/*ExprCommaSeparatedWithoutInitializer(expressionWithoutInitializer expression)
*/

othersInit = ExprOtherIfWithoutInitializer(exprInit, expr2, expr3) -> [typecheck: exprInit expr2 expr3
	{ if(exprInit->check!=BOOL) { yyerror("Non-Bool"); } /*TODO: weiter..?*/
	}
];

/*| ExprAll2(type)
*/
