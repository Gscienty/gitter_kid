import React from 'react';
import ProjectTemplate from './_project';
import { connect } from 'react-redux';
import HighLight from 'react-syntax-highlight';
import { Card } from 'antd';

class Page extends ProjectTemplate {
    state = { prefixURI: '', path: '', type: 'unknow' }
    
    freshBlob(path, branchName) {
        this.props.dispatch({
            type: 'blob/getTextBlob',
            payload: {
                repositoriesName: this.props.match.params.repositoriesName,
                repositoryName: this.props.match.params.repositoryName,
                branchName,
                path
            }
        });
    }

    freshTree(path, branchName) {
        this.props.history.push(this.state.prefixURI  + '/tree' + path);
        this.props.dispatch({
            type: 'tree/getTree',
            payload: {
                repositoriesName: this.props.match.params.repositoriesName,
                repositoryName: this.props.match.params.repositoryName,
                branchName,
                path
            }
        });
    }

    componentDidMount() {
        this.setState({ prefixURI: this.props.match.url.split('/blob')[0] });

        let pathItems = window.location.hash.substring(1).split('/blob');
        let path = '/';
        if (pathItems.length === 2) {
            path = pathItems[1];
        }

        if (path[path.length - 1] !== '/') {
            path = path + '/';
        }

        if (path !== this.state.path) {
            this.setState({ path }, () => this.freshBlob(path, this.props.match.params.branchName));
        }
    }
    
    transform() {
        let middleCutResult = this.state.path.split('.');
        if (middleCutResult.length > 1) {
            switch (middleCutResult[middleCutResult.length - 1]) {
                case 'cs': return 'csharp';
                case 'html': return 'html';
                case 'md': return 'markdown';
                default: return 'unknow';
            }
        }
        return 'unknow';
    }

    renderPath() {
        let pathItems = this.state.path.substring(0, this.state.path.length - 1).split('/');
        pathItems[0] = { name: '', path: '' };
        for (let i = 1; i < pathItems.length; i++) {
            pathItems[i] = { name: pathItems[i], path: pathItems[i - 1].path + '/' + pathItems[i] };
        }

        pathItems[0].name = this.props.match.params.repositoryName;
        pathItems[0].path = '/';

        return pathItems.map((item, index) => <span style={{ fontSize: 21 }} key={ index }>
            { index === 0 ? '' : '/' }
            <a onClick={() => this.freshTree(item.path, this.props.match.params.branchName)}
            style={{ margin: '0 4px' }}>{ item.name }</a>
        </span>);
    }


    unit() {
        let lines = 0;
        let contentCount = this.props.content.length;
        for (let i = 0; i < contentCount; i++) {
            if (this.props.content[i] === '\n')
            {
                lines += 1;
            }
        }

        return <Card        
            title={ <span>{ this.renderPath() }</span> }
            bodyStyle={{ padding: 0 }}>
            <div style={{ float: 'left' }}>
                <ul style={{
                    listStyle: 'none',
                    textAlign: 'right',
                    padding: '0 5px',
                    marginRight: '12px'
                }}>
                {
                    (() => {
                        let result = [];
                        for(let index = 0; index <= lines; index++) { result.push(index + 1); }
                        return result;
                    })().map(item => <li key={ item }><a>{ item }</a></li>)
                }
                </ul>
            </div>
            <HighLight lang={ this.transform() } value={ this.props.content } />
        </Card>
    }
}

export default connect(
    ({ blob, branches }) => ({
        entryUnit: 'repositories',
        projectEntryUnit: 'code',
        content: blob.content,
        branches: branches.branches
    })
)(Page);