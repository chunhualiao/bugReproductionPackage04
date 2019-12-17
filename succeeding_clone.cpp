#include <rose.h>
#include <Outliner.hh>
#include <stdio.h>

using namespace SageInterface;

template <typename F>
void runTransformationTested(F transformation, const std::string msg,
                             SgProject *project) {
  std::cout << "run ast test before transformation " << msg << std::endl;
  AstTests::runAllTests(project);
  transformation();
  std::cout << "run ast test after transformation " << msg << std::endl;
  AstTests::runAllTests(project);
}

int main(int argc, char **argv) {
  SgProject *project = frontend(argc, argv);

  SgFunctionDeclaration *outlinedFunction =
      SageInterface::findDeclarationStatement<SgFunctionDeclaration>(
          project, "OUT__1__4911__", NULL, true);
  SgFunctionDeclaration *main = SageInterface::findMain(project);
  SgSourceFile *mainFile = SageInterface::getEnclosingSourceFile(main);

  ROSE_ASSERT(main != NULL);
  ROSE_ASSERT(outlinedFunction != NULL);
  ROSE_ASSERT(mainFile != NULL);

  // ... attempt to clone the new outlined function.
  SgFunctionDeclaration *funcCopy;
  runTransformationTested(
      [&project, &funcCopy, &main, &outlinedFunction]() {
        funcCopy = isSgFunctionDeclaration(copyStatement(outlinedFunction));
        funcCopy->set_name(funcCopy->get_name() + "_COPY");
        // Insert it to a scope
        SgGlobal *glb = getFirstGlobalScope(project);
        insertStatement(main, funcCopy, false, false);
      },
      "[function cloning]", project);

  return 0;
}
