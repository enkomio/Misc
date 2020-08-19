// --------------------------------------------------------------------------------------
// Sacara FAKE build script
// --------------------------------------------------------------------------------------
#load ".fake/build.fsx/intellisense.fsx"

open System
open System.Reflection
open System.Text
open System.IO
open Fake.DotNet
open Fake.Core
open Fake.IO
open Fake.Core.TargetOperators
open Fake.IO.Globbing.Operators
 
// The name of the project
let project = "Misc projects"

// Short summary of the project
let description = "A collection of misc projects."

// List of author names (for NuGet package)
let authors = [ "Enkomio" ]

// Build dir
let buildDir = Path.GetFullPath("build")

// Package dir
let deployDir = Path.GetFullPath("deploy")

// project list
let projects = [        
    ("SacaraAsm.fsproj", None)
    ("ES.SacaraVm.fsproj", None)
]

        
(*
    FAKE targets
*)
Target.create "Clean" (fun _ ->
    nativeProjects
    |> List.iter(fun (projectFile, _) ->
        let projName = Path.GetFileNameWithoutExtension(projectFile)
        let projReleaseDir = Path.Combine(projName, "Release")
        Shell.cleanDir projReleaseDir
    )

    Shell.cleanDir buildDir
    Directory.ensure buildDir

    Shell.cleanDir deployDir
    Directory.ensure deployDir
)

Target.create "Compile" (fun _ ->
    let build(project: String, buildDir: String) =        
        let fileName = Path.GetFileNameWithoutExtension(project)
        let buildAppDir = Path.Combine(buildDir, fileName)
        Directory.ensure buildAppDir
        
        // build the project
        [project]
        |> MSBuild.runRelease id buildAppDir "ReBuild"
        |> Trace.logItems "AppBuild-Output: "
    
    // build all projects
    try
        projects
        |> List.map(fun projName ->
            let projDir = Path.GetFileNameWithoutExtension(projName)
            let projFile = Path.Combine(projDir, projName)
            projFile
        )
        |> List.iter(fun projectFile -> 
            build(projectFile, buildDir)
        )
    finally
        restoreBuildOptions()
)

Core.Target.create "Release" (fun _ ->
    let releaseDirectory = Path.Combine(releaseDir, String.Format("Misc.v{0}", releaseVersion))
    Directory.CreateDirectory(releaseDirectory) |> ignore

    // copy all files in the release dir    
    projects
    |> List.map(fun projName -> Path.Combine(buildDir, projName))
    |> List.iter(fun projDir -> 
        Shell.copyDir releaseDirectory projDir (fun _ -> true)
    )
    
    // create zip file
    let buildDirectory = Path.Combine(buildDir, "Misc")
    let releaseFilename = releaseDirectory + ".zip"
    Directory.GetFiles(releaseDirectory, "*.*", SearchOption.AllDirectories)
    |> Array.filter(fun file ->
        [".pdb"] 
        |> List.contains (Path.GetExtension(file).ToLowerInvariant())
        |> not
    )
    |> Fake.IO.Zip.zip releaseDirectory releaseFilename
)

"Clean"
    ==> "Compile"
    ==> "Release"

// start build
Core.Target.runOrDefault "Release"