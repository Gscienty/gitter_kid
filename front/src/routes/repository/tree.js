import React from 'react';
import ProjectTemplate from './_project';
import { Table, Card } from 'antd';
import { connect } from 'react-redux';
import  FolderIcon from 'react-icons/lib/io/folder';
import  FileIcon from 'react-icons/lib/go/file-code';
import CodeTemplate from './_code';

function __getTreePathFromHash () {
    let treeUriPosition = window.location.hash.indexOf ('/tree');
    let path = window.location.hash.substring (treeUriPosition + '/tree'.length);
    return path;
}

class Page extends ProjectTemplate {
    state = { initialized: false }

    freshTreeItems () {
        this.props.dispatch({
            type: 'tree/getTree',
            payload: {
                repositoriesName: this.props.match.params.repositoriesName,
                repositoryName: this.props.match.params.repositoryName,
                branchName: this.props.match.params.branchName,
                path: __getTreePathFromHash ()
            },
            after: () => this.setState({ initialized: true })
        });
    }

    componentWillMount () { this.freshTreeItems (); }

    componentWillReceiveProps() { this.setState ({ initialized: false }, () => this.freshTreeItems ()); }

    waitingRender () {
        return <span>waiting...</span>;
    }

    treeItemOnClick (record) {
        this.setState({ initialized: false }, () => this.props.dispatch({
            type: 'tree/enterSubTree',
            payload: {
                ...record,
                repositoriesName: this.props.match.params.repositoriesName,
                repositoryName: this.props.match.params.repositoryName,
                branchName: this.props.match.params.branchName
            },
            after: () => this.setState({ initialized: true })
        }));
    }

    renderPath() {
        return  <div style={{ overflow: 'hidden', textOverflow: 'ellipsis' }}>{
            this.props.treePathItems.map((item, index) => <span style={{ fontSize: 18 }} key={ index }>
                { index === 0 ? '' : '/' }
                <span style={{ margin: '0 4px' }}>
                    {
                        index === this.props.treePathItems.length - 1 && index !== 0
                        ? <span style={{ color: '#00000' }}>{ item.name }</span>
                        : <a
                            style={{ color: '#0366d6' }}
                            onClick={() => this.treeItemOnClick ({ path: item.path, type: 'Tree' }) }>
                            { item.name }
                        </a>
                    }
                </span>
            </span>)
        }</div>;
    }

    displayItemName (name) {
        if (name.indexOf ('/') !== -1) {
            let lastPosition = name.lastIndexOf('/');
            return  <span style={{ color: '#0366d6' }}>
                <span style={{ color: '#6a737d' }}>{ name.substring(0, lastPosition) + '/'}</span>
                { name.substring(lastPosition + 1) }
            </span>;

        }
        return <span style={{ color: '#0366d6' }}>{ name }</span>;
    }

    renderReadmeBlock () {
        let markdowns = this.props.treeItems.filter(item => item.name.endsWith ('.md'));
        if (markdowns.length === 0) {
            return null;
        }
        
        return <CodeTemplate
            dispatch={ this.props.dispatch }
            repositoriesName={ this.props.match.params.repositoriesName }
            repositoryName={ this.props.match.params.repositoryName }
            branchName={ this.props.match.params.branchName }
            path={ markdowns[0].path }
            name={ markdowns[0].name }
            content={ this.props.readmeContent }
        />;
    }

    treeRender () {
        return <div>
            <Card
                title={ this.renderPath () }
                bodyStyle={{ padding: 0 }}>
                <Table
                    columns={[{
                        key: 'fileName',
                        render: (text, record) => <a onClick={ () => this.treeItemOnClick (record) }>
                            <span style={{ fontSize: 18 }}>{ record.type === 'Blob' ? <FileIcon /> : <FolderIcon /> }</span>
                            <span style={{ marginLeft: 8 }}>{ this.displayItemName (record.name) }</span>
                        </a>
                    }]}
                    dataSource={ this.props.treeItems.map(e => ({ ...e, key: e.name })) }
                    showHeader={ false }
                    pagination={ false }
                    size="small"
                />
            </Card>
            <div style={{ marginTop: '21px' }}>
                { this.renderReadmeBlock () }
            </div>
        </div>;
    }

    unit() {
        return this.state.initialized ? this.treeRender() : this.waitingRender();
    }
}

export default connect(
    ({ tree, blob, branches }) => ({
        entryUnit: 'repositories',
        projectEntryUnit: 'code',
        treeItems: tree.items,
        treePath: tree.path,
        treePathItems: tree.pathItems,
        readmeContent: blob.content,
        branches: branches.branches
    })
)(Page);