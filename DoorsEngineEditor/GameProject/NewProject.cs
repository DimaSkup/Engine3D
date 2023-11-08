using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Diagnostics;
using System.Runtime.Serialization;


namespace DoorsEngineEditor.GameProject
{
    [DataContract]
    public class ProjectTemplate
    {
        [DataMember]
        public string ProjectType { get; set; }
        public string ProjectFile { get; set; }
        public List<string> Folders { get; set; }
    }

    class NewProject : ViewModelBase
    {
        // constructor
        public NewProject()
        {
            try
            {
                var templatesFiles = Directory.GetFiles(templatePath_, "template.xml", SearchOption.AllDirectories);
                Debug.Assert(templatesFiles.Any());

                foreach(var file in templatesFiles)
                {
                    var template = new ProjectTemplate()
                    {
                        ProjectType = "Empty Project",
                        ProjectFile = "project.doors_engine",
                        Folders = new List<string>() { ".DoorsEngine", "Content", "GameCode"}
                    };
                }
            }
            catch(Exception e)
            {
                Debug.WriteLine(e.Message);
            }

        }
     
        public string Name
        {
            get { return name_; }
            set 
            {
                if (name_ != value)
                {
                    name_ = value;
                    OnPropertyChanged(nameof(Name));
                }
            }
        }
       
        public string Path
        {
            get { return path_; }
            set
            {
                if (path_ != value)
                {
                    path_ = value;
                    OnPropertyChanged(nameof(Path));
                }
            }
        }


        //
        // VARS
        //

        // field NAME
        private string name_ = "NewProject";

        // field PATH
        private string path_ = $@"{Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)}\DX11Framework\";

        private readonly string templatePath_ = @"..\..\DoorsEngineEditor\ProjectTemplates";
    }
}
