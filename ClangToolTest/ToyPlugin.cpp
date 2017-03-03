//
//  ToyPlugin.cpp
//  ClangToolTest
//
//  Created by jingsong.jn on 2017/3/2.
//  Copyright © 2017年 jingsong.jn. All rights reserved.
//

#include <iostream>
// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTConsumer.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"
#include "clang/Frontend/ASTUnit.h"
#include "gtest/gtest.h"

using namespace std;
using namespace clang::tooling;
using namespace clang;
using namespace llvm;

static llvm::cl::OptionCategory MyToolCategory("ToyPlugin options");

namespace ToyPlugin{
    class ToyClassVisitor : public RecursiveASTVisitor<ToyClassVisitor>
    {
    public:
        explicit ToyClassVisitor(ASTContext *Context): Context(Context) {
        }
        
        bool VisitObjCInterfaceDecl(ObjCInterfaceDecl *declaration)
        {
            printf("ObjClass: %s\n", declaration->getNameAsString().c_str());
            return true;
        }
    private:
        ASTContext *Context;
    };
    
    class ToyConsumer : public clang::ASTConsumer
    {
    public:
        explicit ToyConsumer(ASTContext *Context): visitor(Context) {
        
        }
        
        virtual void HandleTranslationUnit(ASTContext &context) {
            cout<<"test"<<endl;
            visitor.TraverseDecl(context.getTranslationUnitDecl());
        }
    private:
        ToyClassVisitor visitor;
    };
    
    class ToyASTAction : public PluginASTAction
    {
    public:
        unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler, llvm::StringRef InFile)
        {
//            return llvm::make_unique<ToyConsumer>();
            return std::unique_ptr<clang::ASTConsumer>(new ToyConsumer(&Compiler.getASTContext()));
        }
        
        bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string>& args) {
            return true;
        }
    };
};

int main(int argc, const char * argv[]) {
    
    CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());
    return Tool.run(newFrontendActionFactory<ToyPlugin::ToyASTAction>().get());
}
