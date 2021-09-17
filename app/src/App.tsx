import React,{useState} from 'react';
import { message,Button,Image ,Table,Upload} from 'antd';
import ProList from '@ant-design/pro-list';
import { DownloadOutlined,DeleteOutlined,FolderOutlined,FileOutlined,InboxOutlined} from '@ant-design/icons';
const {Dragger}=Upload;

const App =function (){ 
  const [loading,setLoading]=useState(true);
  const currentDir=GetQueryString(window.location.search,"dir");
  const [dataSource,setDataSource]=useState([]);
  function GetQueryString(paraPart:string,name:string) {
    var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)");
    var r = paraPart.substr(1).match(reg);
    if (r != null) return unescape(r[2]); return "";
}
const props = {
  name: 'file',
  multiple: true,
  action: '/api/upload?folder='+currentDir,
  onChange:(info:any)=> {
    const { status } = info.file;
    if (status !== 'uploading') {
      //console.log(info.file, info.fileList);
    }
    if (status === 'done') {
      message.success(`${info.file.name} file uploaded successfully.`);
    } else if (status === 'error') {
      message.error(`${info.file.name} file upload failed.`);
    }
  },
  onDrop:(e:any)=>{
    //console.log('Dropped files', e.dataTransfer.files);
  },
};
  if (loading)
  {
    fetch('/api/dir?dir='+currentDir,{
      method:'GET'
    }).then((response)=>{
      return response.json();
    }).then((data)=>{
      for (var k of data)
        k.image=k.type===4?<FolderOutlined/>:<FileOutlined/>
      setLoading(false);
      setDataSource(data);
    })
  }
  return (
    <>
    <Dragger {...props}>
    <p className="ant-upload-drag-icon">
      <InboxOutlined />
    </p>
    <p className="ant-upload-text">文件上传</p>
    <p className="ant-upload-hint">
    点击此处或拖动文件到此处
    </p>
  </Dragger>
    <ProList<any>
    onRow={(record: any) => {
      return {
        onClick: () => {
          if (record.type===4)
          {
            window.location.href="/index.html?dir="+currentDir+record.name+"/";
          }
          if (record.type===8)
          {
            window.location.href="/file?file="+currentDir+record.name;
          }
        },
      };
    }}
    rowKey="name"
    headerTitle="文件列表"
    loading={loading}
    dataSource={dataSource}
    showActions="hover"
    showExtra="hover"
    metas={{
      title: {
        dataIndex: 'name',
      },
      avatar: {
        dataIndex: 'image',
      },
      actions: {
        render: (text, row) => [
          <Button type="primary" shape="round" icon={<DeleteOutlined/>} onClick={()=>{
              fetch('/api/remove?file='+currentDir+row.name).then((response)=>{
                return response.json();
              }).then((data)=>{
                if (data.status==="success")
                {
                  message.info("删除成功")
                  setTimeout(() => {
                    window.location.reload();
                  }, 2000);
                }
                else message.error("删除失败")
              })
          }}/>,
          <Button type="primary" shape="round" icon={<DownloadOutlined />} onClick={()=>{
            if (row.type===4)
            {
              window.location.href="/folder?folder="+currentDir+row.name+"/";
            }
            if (row.type===8)
            {
              window.location.href="/file?file="+currentDir+row.name;
            }
            }} />
        ],
        search: false,
      },
      }}
      
      />
      </>
);
  }
export default App;
