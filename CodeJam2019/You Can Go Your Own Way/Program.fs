open System
open System.Collections.Generic

type State = {
    Row: Int32
    Column: Int32
}

type Move = {
    StartState: State
    EndState: State
    Direction: Char
}

let toNextState(curState: State, direction: Char) =
    if direction = 'S'
    then {Row=curState.Row+1; Column=curState.Column}
    else {Row=curState.Row; Column=curState.Column+1}

let toMove(curState: State, direction: Char) = {
    StartState = curState
    EndState = toNextState(curState, direction)        
    Direction = direction
}
    
let computeLidiaMoves(lidia: Char array) = [
    let mutable curState = {Row=0; Column=0}
    for direction in lidia do
        let move = toMove(curState, direction)
        curState <- move.EndState
        yield move
]

let getOppositeDirection(direction: Char) =
    if direction = 'S'
    then 'E'
    else 'S'

let getBestDirection(N: Int32, state: State) =
    if state.Row + 1 >= N then 'E'
    elif state.Column + 1 >= N then 'S'
    elif state.Row <= state.Column then 'S'
    else 'E'

let tryGetLidiaState(state: State, lidiaMoves: Move list) =
    lidiaMoves |> Seq.tryFind(fun m -> m.StartState = state)

let isInvalid(moves: Stack<Move>, lidiaMoves: Move list) =
    if moves.Count > 0 then
        let myMove = moves.Peek()
        let prevState = myMove.StartState

        match tryGetLidiaState(prevState, lidiaMoves) with
        | Some lidiaMove when lidiaMove = myMove -> true
        | _ -> false
    else    
        false

let isOutside(N: Int32, state: State) =
    state.Row >= N || state.Column >= N

let completed(N: Int32, state: State) =
    state.Row = N-1 && state.Column = N-1

let rec findSolution(N: Int32, state: State, moves: Stack<Move>, lidiaMoves: Move list) =    
    if isOutside(N, state) then
        moves.Pop() |> ignore
        false
    elif isInvalid(moves, lidiaMoves) then
        moves.Pop() |> ignore
        false
    elif completed(N, state) then
        true
    else
        let direction = getBestDirection(N, state)
        let moveDirection = toMove(state, direction)
        moves.Push(moveDirection)
        if findSolution(N, moveDirection.EndState, moves, lidiaMoves) |> not then  
            let oppositeDirection = getOppositeDirection(direction)
            let moveOppositeDirection = toMove(state, oppositeDirection)
            moves.Push(moveOppositeDirection)
            if findSolution(N, moveOppositeDirection.EndState, moves, lidiaMoves) |> not then
                moves.Pop() |> ignore
                false
            else
                true
        else
            true
    

let runSample(N: Int32, lidia: String) =
    let lidiaMoves = computeLidiaMoves(lidia.ToCharArray())

    let myState = {Row=0; Column=0}
    let myMoves = new Stack<Move>()

    // backtracking FTW
    findSolution(N, myState, myMoves, lidiaMoves) |> ignore

    myMoves
    |> Seq.rev
    |> Seq.map(fun m -> m.Direction)
    |> fun l -> String.Join(String.Empty, l)
    
[<EntryPoint>]
let main argv = 

    let testCases = Int32.Parse(Console.ReadLine().Trim())
    for i=0 to testCases-1 do
        let N = Int32.Parse(Console.ReadLine().Trim())

        let lidia = Console.ReadLine().Trim()     
        let solution = runSample(N, lidia)
        Console.WriteLine("Case #{0}: {1}", i+1, solution)
    0
