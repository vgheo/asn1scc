module BackendAst

open System
open System.Numerics
(*
type TypeReference = Lazy<TypeDefinition>
  
and InnerType =
    | TypeReference of TypeReference
    | BASE_Integer          
    | BASE_Real
    | BASE_NullType
    | BASE_Boolean 

and TypeDefinition = {
    name:string
    asn1Type:Asn1TypeKind
    baseType: TypeReference option      // indicates that this type extends the base type
}

and IntegerTypeDefintion = {
    name:string
    uPEREncClass : string
    acnEncClass : string
    baseType: Lazy<IntegerTypeDefintion> option      // indicates that this type extends the base type
}
and IntegerValue   = 
    | IntLiteral                of IntegerTypeDefintion*BigInteger
    | IntegerValueReference     of ValueReference*Lazy<IntegerValue>


and RealTypeDefintion = {
    name:string
    uPEREncClass : string
    acnEncClass : string
    baseType: Lazy<RealTypeDefintion> option      // indicates that this type extends the base type
}
and RealValue   = 
    | RealLiteral            of RealTypeDefintion*double
    | RealValueReference     of ValueReference*Lazy<RealValue>



and Asn1TypeKind =
    //| Integer           of Integer
    | Real
    | IA5String
    | NumericString
    | OctetString 
    | NullType
    | BitString
    | Boolean 
    | Sequence          of list<ChildInfo>

     
and ChildInfo = {
        name:string
        Type:InnerType
    }

and ValueReference = String*String


*)


type BitStringType = 
    | FixBitStringType  of (int*int)  // bytes/bits
    | VarBitStringType  of (int*int)  // bytes/bits


type ChoiceTypeItem = {
    name : string
    typeDefinitionBody : TypeDefinitionBody
}
    
and ChoiceType = {
    sChoiceIDForNone : string
}

and TypeDefinitionBody =
    | StringType of int
    | Asn1SccSint
    | C_double
    | C_flag
    | C_NullType
    | BitStringType of BitStringType
    | EnumType of (string*int) list
    | ChoiceTypeItem of ChoiceTypeItem


type FunctionParameter = {
    name : string
    typeName : string
    constant : bool
    isPointer : bool
}

type InitializeFunction = {
    name : string
    valueParameter : FunctionParameter
}



type TypeAssignment = {
    name : string
    typeDefinitionBody : TypeDefinitionBody
    requiredBytesForEncoding : Map<Ast.Asn1Encoding, (string*int)>
    initializeFunction : InitializeFunction
}

type SourceModule = {
    typeAssignmens : TypeAssignment   list
}

type SourceFile = {
    name: String
    modules : SourceModule list
}

type Application = {
    files : SourceFile list
}

