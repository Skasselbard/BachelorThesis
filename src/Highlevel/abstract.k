%{KC_TYPES_HEADER
typedef enum{UNDEF,BOOL,NUMB}checkName;
%}

net:
  Net(netName definitionsList places marking transition)
;

netName:
  EmptyNetName()
| NetName(casestring)
;

definitionsList:
  EmptyDefinitionsList()
| DefinitionsList(definitionsList definitions)
;

definitions:
  DefSort(sort)
| DefFunction(function)
| DefConstant(constant)
;

sort:
  Sort(type casestring arrayList)
;

type:
  TypeBool()
| TypeEnum(casestring optionalNumber identList)
| TypeIdent(casestring)
| TypeIntInterval(expression expression)
| TypeStruct(structTypeList)
| TypeInt()
| TypeMultiset(type)
;

optionalNumber:
  EmptyOptNumber()
| OptNumber(integer)
;

identList:
  EmptyIdentList()
| IdentList(identList idents)
;

idents:
  Idents(casestring optionalNumber)
;

structTypeList:
  SingleStructTypeList(structType)
| StructTypeList(structTypeList structType)
;

structType:
  StructType(type casestring arrayList)
;

varOrArray:
 VarOrArray(casestring arrayList)
;

arrayList:
  EmptyArrayList()
| ArrayList(arrayList array)
;

array:
  Array(expression)
;

function:
  Function(type casestring arrayList optFunctionParameters block)
;

optFunctionParameters:
  EmptyOptFunctionParameters()
| OptFunctionParameters(type varOrArray functionParametersList)
;

functionParametersList:
  EmptyFunctionParametersList()
| FunctionParametersList(functionParametersList functionParameters)
;

functionParameters:
  FunctionParameters(type varOrArray)
;

constant: 
  Constant(type casestring arrayList expression)
;

expression:
  ExprAssignment(assignment)
| ExprIncdecrement(incdecrement)
| ExprArithmetic(arithmetic)
| ExprLogical(logical)
| ExprComparison(comparison)
| ExprLeftvalue(leftvalue)
| ExprOther(other)
| ExprInitializerList(initializerList)
| ExprInParenthesis(expression)
{ checkName check = UNDEF;}
;

expressionWithoutInitializer:
  ExprAssignment2(assignment)
| ExprIncdecrement2(incdecrement)
| ExprArithmeticWithoutInitializer(arithmeticWithoutInitializer)
| ExprLogicalWithoutInitializer(logicalWithoutInitializer)
| ExprComparisonWithoutInitializer(comparisonWithoutInitializer)
| ExprLeftvalue2(leftvalue)
| ExprOtherWithoutInitializer(otherWithoutInitializer)
| ExprInParenthesis2(expression)
{ checkName check = UNDEF; }
;

assignment: 
  AssignEqual(leftvalue expression)
| AssignPlus(leftvalue expression)
| AssignMinus(leftvalue expression)
| AssignTimes(leftvalue expression)
| AssignDivide(leftvalue expression)
| AssignMod(leftvalue expression)
{ checkName check = UNDEF; }
;

incdecrement:
  IncrementVal(leftvalue)
| DecrementVal(leftvalue)
| ValIncrement(leftvalue)
| ValDecrement(leftvalue)
;

arithmetic:
  PositiveExpr(expression)
| NegativeExpr(expression)
| ExprAddition(expression expression)
| ExprSubtraction(expression expression)
| ExprMultiplication(expression expression)
| ExprDivision(expression expression)
| ExprModulo(expression expression)
| ExprNumber(integer)
{ checkName check = UNDEF; }
;

arithmeticWithoutInitializer:
  PositiveExpr2(expression)
| NegativeExpr2(expression)
| ExprAdditionWithoutInitializer(expressionWithoutInitializer expression)
| ExprSubtractionWithoutInitializer(expressionWithoutInitializer expression)
| ExprMultiplicationWithoutInitializer(expressionWithoutInitializer expression)
| ExprDivisionWithoutInitializer(expressionWithoutInitializer expression)
| ExprModuloWithoutInitializer(expressionWithoutInitializer expression)
| ExprNumber2(integer)
{ checkName check = UNDEF; }
;

logical:
  NotExpr(expression)
| ExprAnd(expression expression)
| ExprOr(expression expression)
| ExprTrue()
| ExprFalse()
{ checkName check = UNDEF; }
;

logicalWithoutInitializer:
  NotExpr2(expression)
| ExprAndWithoutInitializer(expressionWithoutInitializer expression)
| ExprOrWithoutInitializer(expressionWithoutInitializer expression)
| ExprTrue2()
| ExprFalse2()
{ checkName check = UNDEF; }
;

comparison:
  ExprEquivalent(expression expression)
| ExprNotEqual(expression expression)
| ExprLessThan(expression expression)
| ExprGreaterThan(expression expression)
| ExprLessOrEqual(expression expression)
| ExprGreaterOrEqual(expression expression)
{ checkName check = UNDEF; }
;

comparisonWithoutInitializer:
  ExprEquivalentWithoutInitializer(expressionWithoutInitializer expression)
| ExprNotEqualWithoutInitializer(expressionWithoutInitializer expression)
| ExprLessThanWithoutInitializer(expressionWithoutInitializer expression)
| ExprGreaterThanWithoutInitializer(expressionWithoutInitializer expression)
| ExprLessOrEqualWithoutInitializer(expressionWithoutInitializer expression)
| ExprGreaterOrEqualWithoutInitializer(expressionWithoutInitializer expression)
{ checkName check = UNDEF; }
;

leftvalue:
  LeftValIdent(casestring)       
| LeftValBrackets(leftvalue expression)
| LeftValDot(leftvalue casestring)
;

other:
  ExprIdentParenthesis(casestring) 
| ExprIdentExprInParenthesis(casestring expression)
| ExprCommaSeparated(expression expression)
| ExprOtherIf(expression expression expression)
| ExprAll(type)
{ checkName check = UNDEF; }
;

otherWithoutInitializer:
  ExprIdentParenthesis2(casestring) 
| ExprIdentExprInParenthesis2(casestring expression)
| ExprCommaSeparatedWithoutInitializer(expressionWithoutInitializer expression)
| ExprOtherIfWithoutInitializer(expressionWithoutInitializer expression expression)
| ExprAll2(type)
{ checkName check = UNDEF; }
;

initializerList:
  Braces()
| ExprInBraces(expression)
| ExprInBracesColon(expression expression expressionListColon)
;

expressionListColon:
  EmptyExpressionListColon()
| ExpressionListColon(expression expression expressionListColon)
;

places:
  Places(optSafe optType place commaPlaces morePlaces)
;

optSafe:
  EmptySafe()
| Safe(optNumber)
;

optNumber:
  EmptyPlaceNumber()
| PlaceNumber(integer)
;

optType:
  EmptyPlaceType()
| PlaceTypeBool()
| PlaceTypeEnum(casestring optionalNumber identList)
| PlaceTypeIntInterval(expression expression)
| PlaceTypeStruct(structTypeList)
| PlaceTypeInt()
| PlaceTypeMultiset(type)
;

place:
  Place(casestring)
| PlaceWithTypeIdent(casestring casestring)
;

commaPlaces:
  EmptyCommaPlaces()
| CommaPlaces(optType place commaPlaces)
;

morePlaces:
  EmptyMorePlaces()
| MorePlaces(optSafe optType place commaPlaces morePlaces)
;

marking:
  EmptyMarking()
| Marking(casestring expression markingList)
;

markingList:
  EmptyMarkingList()
| MarkingList(casestring expression markingList)
;

transition:
  EmptyTransition()
| Transition(casestring fairness variable guard optIdentExprList optIdentExprList transition)
;

fairness:
  EmptyFairness()
| WeakFair()
| StrongFair()
;

variable:
  EmptyVariable()
| Variable(type varOrArray varOrArrayList variable)
;

varOrArrayList:
  EmptyVarOrArrayList()
| VarOrArrayList(varOrArray varOrArrayList)
;

guard:
  EmptyGuard()
| Guard(expression)
;

optIdentExprList:
  EmptyOptIdentExprList()
| OptIdentExprList(casestring expression identExprList)
;

identExprList:
  EmptyIdentExprList()
| IdentExprList(casestring expression identExprList)
;

block:
  Block(declarationOrStatement)
;

declarationOrStatement:
  EmptyDeclarationOrStatement()
| DeclOrStatemDeclaration(declaration declarationOrStatement)
| DeclOrStatemStatement(statement declarationOrStatement)
;

declaration: 
  Declaration(type varOrArray varOrArrayList)
;

statement:
  StatementSemiolon()
| StatementBlock(block)
| StatementExprSemicolon(expressionWithoutInitializer)
| StatementIf(ifStatement)
| StatementWhile(expression statement)
| StatementDoWhile(statement expression)
| StatementForExpr(expression expression expression statement)
| StatementForTypeExpr(type casestring expression expression expression statement)
| StatementForIdentColon(casestring expression statement)
| StatementForAll(type casestring type)
| StatementSwitch(expression switchCase)
| StatementBreak()
| StatementContinue()
| StatementReturn(expression)
;

ifStatement:
  IfStatement(expression statement optElse)
;

optElse:
  OptElseEmpty()
| OptElse(statement)
;

switchCase:
  SwitchCase(expression statement switchCaseList)
| SwitchDefault(statement switchCaseList)
;

switchCaseList:
  EmptySwitchCaseList()
| SwitchCaseList(expression statement switchCaseList)
| SwitchDefaultList(statement switchCaseList)
;
